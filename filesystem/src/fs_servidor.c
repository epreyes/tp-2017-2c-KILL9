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

		if (recv(new_fd, &d, sizeof(t_header), 0) < 0) {
			log_error(logger, "Error en la recepcion del handshake");
			exit(1);
		}

		log_debug(logger, "Se recibio handshake %d", d.idMensaje);
		codigoHandshake = d.idMensaje;

		if (codigoHandshake != YAMA_HSK && codigoHandshake != NODE_HSK) {
			log_error(logger, "Codigo incorrecto de Handshake.");
			continue;
		}

		// Creo thread cliente

		pthread_t hilo_cliente;

		if (codigoHandshake == NODE_HSK) {

			// TODO: debe controlarse para la inicializacion, que los nodos que se conecten sean de:
			// Si hay estado anterior: que sean los del estado anterior, denegar cualquier otro
			// Si no hay estado anterior: pueden ser cualquiera, deben agregarse a nodos.bin

			int hsk_ok = HSK_OK;

			if (send(new_fd, &hsk_ok, sizeof(int), 0) < 0) {
				log_error(logger, "Error en el envio de hsk ok");
				exit(1);
			}

			log_info(logger, "Nuevo nodo conectado");

			if (pthread_create(&hilo_cliente, NULL, connection_handler_nodo,
					(void*) new_fd) < 0) {
				perror("Error creando thread cliente nodo");
				exit(1);
			}

			// Agrego el nodo en memoria

			t_nodo* nodo = malloc(sizeof(t_nodo));
			nodo->id = new_fd; // Debe venir del nodo
			nodo->libre = 5500;  // inicialmente es el total
			nodo->total = 5500; // TODO: debe venir del nodo
			nodo->direccion = "127.0.0.2:6003"; // TODO: debe venir del nodo
			nodo->socketNodo = new_fd;

			list_add(nodos->nodos, nodo);

			// Guardo nodo en nodos.bin
			guardarConfigNodoEnBin();

			crearBitMapBloquesNodo(nodo);

		}

		// TODO: controlar que no lleguen dos yamas
		if (codigoHandshake == YAMA_HSK) {

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
		header.idMensaje=cod_op;

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

		log_info(logger, "Pidiendo informacion del archivo: %s (long: %d)", buffer, pedido.size);

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

				biYama->block_id = i;

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

void guardarConfigNodoEnBin() {

	t_config* nodoConfig = malloc(sizeof(t_config));
	nodoConfig = config_create("/home/utnso/SO/fs/bin/metadata/nodos.bin");

	// Si no hay estado anterior, se guarda el nodo en nodos.bin

	if (nodoConfig == NULL) {
		int conf = config_save_in_file(nodoConfig,
				"/home/utnso/SO/fs/bin/metadata/nodos.bin");
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
