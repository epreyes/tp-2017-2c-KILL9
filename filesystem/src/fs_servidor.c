/*
 * fs_servidor.c

 *
 *  Created on: 15/10/2017
 *      Author: utnso
 */

#include "fs_servidor.h"

void* lanzarHiloServidor() {

	int sockfd, new_fd; // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del cliente
	int sin_size;
	int yes = 1;

	uint16_t codigoHandshake = 0;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(fs->puerto); // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	log_info(logger,
			"YamaFS inicializado y esperando conexiones en el puerto %d",
			fs->puerto);
	while (1) {  // main accept() loop
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size))
				== -1) {
			perror("accept");
			continue;
		}
		log_info(logger, "server: nuevo cliente conectado desde %s\n",
				inet_ntoa(their_addr.sin_addr));

		t_header d;
		int handshak = 0;

		if (recv(new_fd, &handshak, sizeof(int), 0) < 0) {
			log_error(logger, "Error en la recepcion del handshake");
			exit(1);
		}

		log_debug(logger, "Se recibio handshake %d", handshak);
		codigoHandshake = handshak;

		if (codigoHandshake != YAMA_HSK && codigoHandshake != NODE_HSK
				&& codigoHandshake != WORKER_HSK) {
			log_error(logger, "Codigo incorrecto de Handshake.");
			continue;
		}

		// Creo thread cliente

		pthread_t hilo_cliente;

		if (codigoHandshake == NODE_HSK) {
			int block_quantity = 0;
			int id_nodo = 0;
			int data_size = 0;

			recv(new_fd, &block_quantity, sizeof(int), 0);
			recv(new_fd, &id_nodo, sizeof(int), 0);
			recv(new_fd, &data_size, sizeof(int), 0);
			char* worker = malloc(data_size);

			recv(new_fd, worker, sizeof(char) * data_size, 0);

			// TODO: no permitir que se conecte el mismo nodo mas de una vez

			t_nodo* nodo;

			if (vieneDeNoEstado == 1) {

				int hsk_ok = HSK_OK;

				if (send(new_fd, &hsk_ok, sizeof(int), 0) < 0) {
					log_error(logger, "Error en el envio de hsk ok");
					exit(1);
				}

				if (pthread_create(&hilo_cliente, NULL, connection_handler_nodo,
						(void*) new_fd) < 0) {
					perror("Error creando thread cliente nodo");
					exit(1);
				}

				// Agrego el nodo en memoria
				t_nodo* nodo = malloc(sizeof(t_nodo));
				nodo->id = id_nodo;
				nodo->libre = block_quantity;  // inicialmente es el total
				nodo->total = block_quantity;

				nodo->direccion = malloc(data_size + 1);
				memcpy(nodo->direccion, worker, data_size);
				nodo->direccion[data_size] = '\0';

				nodo->socketNodo = new_fd;

				log_info(logger,
						"Nuevo nodo conectado - id: %d - total bloques: %d - direccion worker: %s",
						id_nodo, block_quantity, worker);

				sem_wait(&listaNodos);
				list_add(nodos->nodos, nodo);
				sem_post(&listaNodos);

				// Guardo nodo en nodos.bin
				guardarConfigNodoEnBin();

				// Genero bitmap de gestion de bloques
				crearBitMapBloquesNodo(nodo);

				// Solo si viene de un no estado
				sem_post(&nodoInit);

			} else {

				t_nodo* nodoAnterior = nodoPerteneceAEstadoAnterior(nodo);

				if (nodoAnterior == NULL) {
					// Rechazo conexion de nodo
					int hsk = ACCESODENEGADO;

					if (send(new_fd, &hsk, sizeof(int), 0) < 0) {
						log_error(logger, "Error en el envio de hsk");
						exit(1);
					}
					close(new_fd);
				} else {
					int hsk_ok = HSK_OK;

					if (send(new_fd, &hsk_ok, sizeof(int), 0) < 0) {
						log_error(logger, "Error en el envio de hsk ok");
						exit(1);
					}

					if (pthread_create(&hilo_cliente, NULL,
							connection_handler_nodo, (void*) new_fd) < 0) {
						perror("Error creando thread cliente nodo");
						exit(1);
					}

					// Agrego el nodo en memoria

					// TODO: debe tomar la info de data.bin

					t_nodo* nodo = malloc(sizeof(t_nodo));
					nodo->id = id_nodo;
					nodo->libre = block_quantity;  // inicialmente es el total
					nodo->total = block_quantity;
					nodo->direccion = malloc(data_size + 1);
					memcpy(nodo->direccion, worker, data_size);
					nodo->direccion[data_size] = '\0';
					nodo->socketNodo = new_fd;

					log_info(logger,
							"Nodo de estado anterior conectado - id: %d - total bloques: %d - direccion worker: %s",
							id_nodo, block_quantity, worker);

					sem_wait(&listaNodos);
					list_add(nodos->nodos, nodo);
					sem_post(&listaNodos);

					// Habilito bloques de archivos de un estado anterior
					habilitarBloques(nodo);

				}

			}

		}

		if (codigoHandshake == WORKER_HSK) {

			if (estadoEstable == 0) {
				log_error(logger,
						"El fs no se encuentra en estado estable, rechazando conexion de worker");
				close(new_fd);
			} else
				log_info(logger, "Worker conectado");

			if (pthread_create(&hilo_cliente, NULL, connection_handler_worker,
					(void*) new_fd) < 0) {
				perror("Error creando thread cliente worker");
				exit(1);
			}
		}

		// TODO: controlar que no lleguen dos yamas
		if (codigoHandshake == YAMA_HSK) {

			if (estadoEstable == 0) {
				log_error(logger,
						"El fs no se encuentra en estado estable, rechazando conexion de yama");
				close(new_fd);
			} else
				log_info(logger, "Yama conectado");

			if (pthread_create(&hilo_cliente, NULL, connection_handler_yama,
					(void*) new_fd) < 0) {
				perror("Error creando thread cliente yama");
				exit(1);
			}
		}

	}

}

// Handler de conexiones de clientes

void *connection_handler_nodo(void *socket_desc) {
	int socketCliente = (int) socket_desc;
	int32_t bytes = 0;
	t_header header;

	while (1) {

		log_info(logger, "Esperando mensaje del cliente %d...", socketCliente);
		bytes = recv(socketCliente, &header, sizeof(int), 0);

		if (bytes <= 0) {
			log_info(logger, "Error: el cliente %d cerro la conexion",
					socketCliente);

			int nodoId = buscarNodoPorSocket(socketCliente);

			// TODO: Buscar los archivos que tienen referencia a este nodo y eliminarle una instancia
			// ****************************+
			int i = 0;
			t_directorio* dir = inicioTablaDirectorios;

			dir++;

			for (i = 1; i < MAX_DIR_FS; i++) {

				if (dir->padre == -1 && dir->indice != 0)
					continue;

				t_list* archivosInit = (t_list*) listarArchivos(dir->nombre);
				if (list_size(archivosInit) == 0)
					continue;

				int j = 0;

				for (j = 0; j < list_size(archivosInit); j++) {

					char* arch = list_get(archivosInit, j);

					char* pathMetadata = string_new();
					string_append(&pathMetadata, fs->m_archivos);
					string_append(&pathMetadata, string_itoa(dir->indice));
					string_append(&pathMetadata, "/");
					string_append(&pathMetadata, arch); // Ya include la extension de la metadata .csv

					t_archivoInit* archivoInit = malloc(sizeof(t_archivoInit));
					archivoInit->bloques = list_create();
					archivoInit->identificador = malloc(
							strlen(pathMetadata) + 1);

					memcpy(archivoInit->identificador, pathMetadata,
							strlen(pathMetadata));

					archivoInit->identificador[strlen(pathMetadata)] = '\0';

					char* mdat = malloc(strlen(pathMetadata) + 1);
					memcpy(mdat, pathMetadata, strlen(pathMetadata));
					mdat[strlen(pathMetadata)] = '\0';

					t_archivoInfo* archInfo = obtenerArchivoInfoPorMetadata(
							pathMetadata);

					int k = 0;
					for (k = 0; k < list_size(archInfo->bloques); k++) {

						t_bloqueInfo* bi = list_get(archInfo->bloques, k);

						// Si el bloque tiene referencia al nodo conectado-> el nro de bloque bi->nroBloque hay que hacerle post
						if (atoi(bi->idNodo0) == nodoId
								|| atoi(bi->idNodo1) == nodoId) {

							// Busco lista de bloques del archivo en listaArchivoInit, luego busco el elemento
							// que se corresponda con el bi->nroBloque y hago post al semaforo
							int y = 0;
							int z = 0;

							for (y = 0; y < list_size(tablaArchivos); y++) {
								t_archivoInit* lb = list_get(tablaArchivos, y);
								if (strcmp(lb->identificador, mdat) == 0) {
									for (z = 0; z < list_size(lb->bloques);
											z++) {
										t_bloqueInit* bli = list_get(
												lb->bloques, z);

										if (bli->nroBloque == bi->nroBloque) {

											bli->cantInstancias -= 1;
											log_info(logger,
													"[%s] - Deshabilitando bloque %d de nodo %d - instancias: %d",
													arch, bli->nroBloque,
													nodoId,
													bli->cantInstancias);
											//sem_post(&bli->semaforo);
										}

									}
								}

							}

						}

					}

				}

				list_destroy(archivosInit);
				dir++;

			}
			// *********
			// Eliminar de lista nodos

			log_info(logger, "Eliminando nodo de lista de nodos");
			sem_wait(&listaNodos);

			int j = 0;
			for (j = 0; j < list_size(nodos->nodos); j++) {
				t_nodo* nod = list_get(nodos->nodos, j);
				if (nod->id == nodoId) {
					list_remove(nodos->nodos, j);
					break;
				}

			}

			sem_post(&listaNodos);

			return 0;
		}

		procesarPedidoNodo(header, socketCliente);
	}

	return 0;
}

void *connection_handler_yama(void *socket_desc) {
	int socketCliente = (int) socket_desc;
	int32_t bytes = 0;
	char cod_op;

	while (1) {

		log_info(logger, "Esperando mensaje del cliente %d...", socketCliente);
		bytes = recv(socketCliente, &cod_op, sizeof(char), 0);

		if (bytes <= 0) {
			log_info(logger, "Error: el cliente %d cerro la conexion",
					socketCliente);
			return 0;
		}

		t_header header;
		header.idMensaje = cod_op;

		procesarPedidoYama(header, socketCliente);
	}

	return 0;
}

void procesarPedidoYama(t_header pedido, int socketCliente) {

	void* respuesta;
	char* buffer;

	t_archivoInfo* info;

	switch (pedido.idMensaje) {

	case 'T':

		recv(socketCliente, &(pedido.size), 4, 0);

		buffer = malloc(pedido.size);

		recv(socketCliente, buffer, pedido.size, 0);

		log_info(logger, "Pidiendo informacion del archivo: %s (long: %d)",
				buffer, pedido.size);

		info = obtenerArchivoInfo(buffer);

		int i = 0;
		int offset = 0;
		int cantReg = 0;
		if (info == NULL) {
			log_error(logger, "No se pudo obtener la info de archivo: %s",
					buffer);
			t_header resp;
			resp.idMensaje = 'E';

			if (send(socketCliente, &resp, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de archivo info a YAMA");
				exit(1);
			}
		}

		else {

			cantReg = list_size(info->bloques);
			respuesta = malloc(sizeof(block_info) * cantReg);

			for (i = 0; i < list_size(info->bloques); i++) {
				t_bloqueInfo* bi = list_get(info->bloques, i);

				block_info* biYama = malloc(sizeof(block_info));

				biYama->block_id = bi->nroBloque;

				biYama->node1_block = bi->idBloque0;
				biYama->node2_block = bi->idBloque1;
				biYama->end_block = bi->finBytes;

				strcpy(biYama->node1_ip, "127.0.0.1");
				biYama->node1_port = 5010;

				strcpy(biYama->node2_ip, "127.0.0.2");
				biYama->node2_port = 5011;

				biYama->node1 = atoi(bi->idNodo0);
				biYama->node2 = atoi(bi->idNodo1);

				memcpy(respuesta + offset, biYama, sizeof(block_info));
				offset += sizeof(block_info);
				free(bi);
				free(biYama);
			}
			list_destroy(info->bloques);
			free(info);

			t_header resp;
			resp.idMensaje = 'O';
			resp.size = cantReg;

			if (send(socketCliente, &resp, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de archivo info a YAMA");
				exit(1);
			}

			if (send(socketCliente, respuesta, sizeof(block_info) * cantReg, 0)
					< 0) {
				log_error(logger,
						"Error en el envio de respuesta de archivo info a YAMA");
				exit(1);
			}

			free(respuesta);

			log_info(logger, "Envio de info archivo a yama ok");

		}

		free(buffer);

		break;

	default:
		// Si es invalido->no hago nada y cierro el socket
		log_error(logger, "Pedido %d invalido", pedido.idMensaje);
		close(socketCliente);
		break;
	}

}

void procesarPedidoNodo(t_header pedido, int socket) {

	char* resultado = malloc(sizeof(char) * pedido.size);

	switch (pedido.idMensaje) {

	case SET_BLOQUE_OK:

		sem_post(&semEscritura);

		log_info(logger, "Escritura en nodo ok");

		break;

	default:
		// Si es invalido->no hago nada y cierro el socket
		log_error(logger, "Pedido %d invalido", pedido.idMensaje);
		close(socket);
		break;
	}

}

void *connection_handler_worker(void *socket_desc) {
	int socketCliente = (int) socket_desc;
	int32_t bytes = 0;
	char cod_op;

	while (1) {

		log_info(logger, "Esperando mensaje del cliente %d...", socketCliente);
		bytes = recv(socketCliente, &cod_op, sizeof(char), 0);

		if (bytes <= 0) {
			log_info(logger, "Error: el cliente %d cerro la conexion",
					socketCliente);
			return 0;
		}

		t_header header;
		header.idMensaje = cod_op;

		procesarPedidoWorker(header, socketCliente);
	}

	return 0;
}

void procesarPedidoWorker(t_header pedido, int socketCliente) {

	char codop;

	int escribir = 0;

	int fileNameSize = 0;
	int fileSize = 0;
	recv(socketCliente, &fileNameSize, sizeof(int), 0);

	char* fileName = malloc(fileNameSize + 1);
	recv(socketCliente, fileName, fileNameSize, 0);
	fileName[fileNameSize] = '\0';

	recv(socketCliente, &fileSize, sizeof(int), 0);

	char* contenido = malloc(fileSize);
	recv(socketCliente, contenido, fileSize, 0);

	switch (pedido.idMensaje) {

	case 'S':

		escribir = escribirArchivo(fileName, contenido, TEXTO);

		if (escribir == 0) {
			log_info(logger, "Escritura de %s realizada con exito", fileName);
			printf("Escritura ok\n");
			codop = 'O';
			if (send(socketCliente, &codop, sizeof(char), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de store final");
				exit(1);
			}
		} else {

			codop = 'E';
			if (send(socketCliente, &codop, sizeof(char), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de store final");
				exit(1);
			}

			switch (escribir) {
			case SIN_ESPACIO:
				log_error(logger,
						"No se pudo escribir el archivo, no hay espacio en disco");
				break;
			case ARCHIVO_EXISTENTE:
				log_error(logger,
						"No se pudo escribir el archivo, el archivo ya existe");
				break;

			case NO_HAY_NODOS:
				log_error(logger, "No hay nodos conectados para la escritura");
				break;

			case TAMANIO_RENGLON_INVALIDO:
				log_error(logger, "Tamanio de renglon es mayor al del bloque");
				break;

			case NO_EXISTE_DIR_DESTINO:
				log_error(logger, "El directorio destino no existe");
				break;
			}

		}

		break;

	default:
		// Si es invalido->no hago nada y cierro el socket
		log_error(logger, "Pedido %d invalido", pedido.idMensaje);
		close(socketCliente);
		break;
	}

}

void guardarConfigNodoEnBin() {

	t_config* nodoConfig = malloc(sizeof(t_config));
	nodoConfig = config_create(fs->m_nodos);

	// Si no hay estado anterior, se guarda el nodo en nodos.bin

	if (nodoConfig == NULL) {
		int conf = config_save_in_file(nodoConfig, fs->m_nodos);
	}

	// Recorro la lista nodos y refresco la config

	int i = 0;
	int totalTamanio = 0;
	int totalLibre = 0;
	char* listaNodos = string_new();
	int cantNodos = list_size(nodos->nodos);
	string_append(&listaNodos, "[");
	for (i = 0; i < cantNodos; i++) {

		t_nodo* nod = list_get(nodos->nodos, i);

		string_append(&listaNodos, "Nodo");
		string_append(&listaNodos, string_itoa(nod->id));

		if (cantNodos == 1)
			string_append(&listaNodos, "]");

		if (cantNodos > 1 && i < (cantNodos - 1))
			string_append(&listaNodos, ",");

		totalTamanio += nod->total;
		totalLibre += nod->libre;

		// Creo la entrada del nuevo nodo

		char* nodoTotalProperty = string_new();
		string_append(&nodoTotalProperty, "Nodo");
		string_append(&nodoTotalProperty, string_itoa(nod->id));
		string_append(&nodoTotalProperty, "Total");

		config_set_value(nodoConfig, nodoTotalProperty,
				string_itoa(nod->total));

		free(nodoTotalProperty);

		char* nodoLibreProperty = string_new();
		string_append(&nodoLibreProperty, "Nodo");
		string_append(&nodoLibreProperty, string_itoa(nod->id));
		string_append(&nodoLibreProperty, "Libre");

		config_set_value(nodoConfig, nodoLibreProperty,
				string_itoa(nod->libre));

		free(nodoLibreProperty);

	}

	if (cantNodos > 1)
		string_append(&listaNodos, "]");

	// Actualizo totales
	config_set_value(nodoConfig, "TAMANIO", string_itoa(totalTamanio));
	config_set_value(nodoConfig, "LIBRE", string_itoa(totalLibre));

	// Actualizo lista NODOS
	config_set_value(nodoConfig, "NODOS", listaNodos);

	config_save(nodoConfig);

	free(listaNodos);

}

// Indica si un nodo que se conecta pertenece a un estado anterior y devuelve su info
t_nodo* nodoPerteneceAEstadoAnterior(t_nodo* nodo) {

	/*t_config* nodoConfig = malloc(sizeof(t_config));

	 nodoConfig = config_create(fs->m_nodos);

	 if (!config_has_property(nodoConfig, "M_DIRECTORIOS")) {
	 fs->m_directorios = config_get_string_value(fs->config,
	 "M_DIRECTORIOS");*/
	t_nodo* nodAnt = malloc(sizeof(t_nodo));

	return nodAnt;
}

// Si no hay estado anterior, acepta el nodo conectado
// Si hay estado anterior, acepta el nodo que se conecta (y solo si existe en nodos.bin),
// y signalea los bloques de los archivos que estan en el nodo indicado
int habilitarBloques(t_nodo* nodo) {

	// Recorro todos los archivos y chequeo cuales poseen una referencia al nodo indicado por parametro
	// Sin importar la instancia, hago signal del bloque del archivo

	int i = 0;
	t_directorio* dir = inicioTablaDirectorios;

	dir++;

	for (i = 1; i < MAX_DIR_FS; i++) {

		if (dir->padre == -1 && dir->indice != 0)
			continue;

		t_list* archivosInit = (t_list*) listarArchivos(dir->nombre);
		if (list_size(archivosInit) == 0)
			continue;

		int j = 0;

		for (j = 0; j < list_size(archivosInit); j++) {

			char* arch = list_get(archivosInit, j);

			char* pathMetadata = string_new();
			string_append(&pathMetadata, fs->m_archivos);
			string_append(&pathMetadata, string_itoa(dir->indice));
			string_append(&pathMetadata, "/");
			string_append(&pathMetadata, arch); // Ya include la extension de la metadata .csv

			t_archivoInit* archivoInit = malloc(sizeof(t_archivoInit));
			archivoInit->bloques = list_create();
			archivoInit->identificador = malloc(strlen(pathMetadata) + 1);

			memcpy(archivoInit->identificador, pathMetadata,
					strlen(pathMetadata));

			archivoInit->identificador[strlen(pathMetadata)] = '\0';

			char* mdat = malloc(strlen(pathMetadata) + 1);
			memcpy(mdat, pathMetadata, strlen(pathMetadata));
			mdat[strlen(pathMetadata)] = '\0';

			t_archivoInfo* archInfo = obtenerArchivoInfoPorMetadata(
					pathMetadata);

			int k = 0;
			for (k = 0; k < list_size(archInfo->bloques); k++) {

				t_bloqueInfo* bi = list_get(archInfo->bloques, k);

				// Si el bloque tiene referencia al nodo conectado-> el nro de bloque bi->nroBloque hay que hacerle post
				if (atoi(bi->idNodo0) == nodo->id
						|| atoi(bi->idNodo1) == nodo->id) {

					// Busco lista de bloques del archivo en listaArchivoInit, luego busco el elemento
					// que se corresponda con el bi->nroBloque y hago post al semaforo
					int y = 0;
					int z = 0;

					for (y = 0; y < list_size(tablaArchivos); y++) {
						t_archivoInit* lb = list_get(tablaArchivos, y);
						if (strcmp(lb->identificador, mdat) == 0) {
							for (z = 0; z < list_size(lb->bloques); z++) {
								t_bloqueInit* bli = list_get(lb->bloques, z);

								if (bli->nroBloque == bi->nroBloque) {

									bli->cantInstancias += 1;
									log_info(logger,
											"[%s] - Habilitando bloque %d de nodo %d - instancias: %d",
											arch, bli->nroBloque, nodo->id,
											bli->cantInstancias);
									sem_post(&bli->semaforo);
								}

							}
						}

					}

				}

			}

		}

		list_destroy(archivosInit);
		dir++;

	}

}

int buscarNodoPorSocket(int socketNodo) {
	int i = 0;
	for (i = 0; i < list_size(nodos->nodos); i++) {
		t_nodo* nod = list_get(nodos->nodos, i);
		if (nod->socketNodo == socketNodo)
			return nod->id;
	}
	return -1;

}
