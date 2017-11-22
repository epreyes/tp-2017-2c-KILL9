/*
 * fs_comandos.c
 *
 * Comandos del fs
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "fs_comandos.h"

// Dado un archivo y un contenido, escribe en el fs.

int escribirArchivo(char* path, char* contenido, int tipo, int tamanio) {

	// Puede que no existan nodos conectados y se invoque a este comando
	if (list_size(nodos->nodos) == 0) {
		return NO_HAY_NODOS;
	}

	char* dirPath = obtenerDirArchivo(path);

	if (!existeDirectorio(dirPath)) {
		return NO_EXISTE_DIR_DESTINO;
	}

	if (existeArchivo(path) == 0) {
		return ARCHIVO_EXISTENTE;
	}

	int bloquesNecesarios;
	if (tipo == TEXTO)
		bloquesNecesarios = obtenerBloquesNecesarios(contenido, tipo, 0);
	if (tipo == BINARIO)
		bloquesNecesarios = obtenerBloquesNecesarios(contenido, tipo, tamanio);

// Debo saber de antemano si con los bloques de los nodos actuales me alcanza para escribir el archivo

	log_debug(logger,
			"Verificando si hay espacio para archivo de %d (*2) bloques...",
			bloquesNecesarios);

	int errorSeleccionNodos = 0;
// Debe ser *2 para hacer las copias
	t_list* bl = obtenerBloquesLibres(bloquesNecesarios * 2,
			&errorSeleccionNodos);

	if (errorSeleccionNodos == -1)
		return ERROR_MISMO_NODO_COPIA;

	if (bl == NULL) {
		return SIN_ESPACIO;
	}

	log_info(logger, "Bloques necesarios para la escritura de %s: %d", path,
			bloquesNecesarios);

	if (bloquesNecesarios == 0) {
		log_error(logger, "Error: no hay contenido para escribir");
		return SIN_CONTENIDO_ESCRIBIR;
	}

	t_list* bloquesInfo = list_create(); // lista con la informacion de bloque que se va a escribir en el archivo de metadata

// Escribo en los bloques reservados

	char bloque[TAMANIO_BLOQUE];

	memset(bloque, '\0', TAMANIO_BLOQUE);

	int offset = 0;

	int d = 0; // tamanio del renglon actual
	int restanteBloque = TAMANIO_BLOQUE;
	int offsetbloque = 0;
	int tamanioArchivo = 0;

	char* p = contenido;

	if (tipo == TEXTO) {

		int i = 0; // Comienzo desde el bloque 0

		t_idNodoBloque* nb = list_get(bl, i);
		int bloqueAModificar = nb->idBloque;
		int idnodo = nb->idNodo;
		long finbytes = 0;

		for (p = contenido + offset; *p; p++) {
			d++;
			tamanioArchivo++;

			if (*p == '\n') {

				//************************************************
				// Logica de escritura de renglon
				//************************************************

				if (d > TAMANIO_BLOQUE) {
					return TAMANIO_RENGLON_INVALIDO;
				}

				if (d <= restanteBloque) {
					// El renglon actual entra en el espacio del bloque disponible
					//finbytes += d;
					;

				} else
				// nuevo bloque
				{

					/// *** Copiado

					//************************************************
					// Decido cual nodo
					//************************************************

					// Selecciono el siguiente nodo (ya preseleccionado)
					nb = list_get(bl, i);
					bloqueAModificar = nb->idBloque;
					idnodo = nb->idNodo;

					int l = 0;
					t_nodo* nodo;

					for (l = 0; l < list_size(nodos->nodos); l++) {
						t_nodo* n = list_get(nodos->nodos, l);
						if (n->id == idnodo) {
							nodo = n;
							break;
						}
					}

					//************************************************
					// Envio bloque al datanode
					//************************************************

					log_debug(logger,
							"Escribiendo renglon en nro de bloque %d id: %d nodo: %d",
							i, bloqueAModificar, idnodo);

					//log_debug(logger, "Contenido: %s", bloque);

					int dataNode = escribirEnDataNode(bloqueAModificar, bloque,
							nodo->socketNodo, finbytes, logger);

					sem_wait(&semEscritura);

					t_bloqueInfo* bi = malloc(sizeof(t_bloqueInfo));

					long bFinBytes = finbytes;

					log_info(logger,
							"Escritura en el datanode realizada con exito");

					// Actualizo lista nodos y nodos.bin
					log_info(logger, "Actualizando nodos.bin...");
					// nodo->libre -= 1; // Ya lo hace la reserva de bloques
					actualizarConfigNodoEnBin(nodo);

					bi->finBytes = finbytes;
					finbytes = 0;
					bi->idBloque0 = bloqueAModificar;
					bi->idNodo0 = string_itoa(nodo->id);

					//*********************************//
					// Copia del bloque
					//*********************************//
					// Debo hacer el envio al datanode del mismo contenido en el nodo y bloque id indicado por la lista (el siguiente elemento)

					// Selecciono el nodo de la copia (ya preseleccionado)
					nb = list_get(bl, i + 1);
					bloqueAModificar = nb->idBloque;
					idnodo = nb->idNodo;

					i++;

					l = 0;

					for (l = 0; l < list_size(nodos->nodos); l++) {
						t_nodo* n = list_get(nodos->nodos, l);
						if (n->id == idnodo) {
							nodo = n;
							break;
						}
					}

					log_debug(logger,
							"Escribiendo renglon (copia) en nro de bloque %d id: %d nodo: %d",
							i, bloqueAModificar, idnodo);

					//log_debug(logger, "Contenido (copia): %s", bloque);

					dataNode = escribirEnDataNode(bloqueAModificar, bloque,
							nodo->socketNodo, bFinBytes, logger);

					sem_wait(&semEscritura);

					log_info(logger,
							"Escritura en el datanode realizada con exito");

					// Actualizo lista nodos y nodos.bin
					log_info(logger, "Actualizando nodos.bin...");
					// nodo->libre -= 1; // Ya lo hace la reserva de bloques
					actualizarConfigNodoEnBin(nodo);

					bi->idBloque1 = bloqueAModificar;
					bi->idNodo1 = string_itoa(nodo->id);

					list_add(bloquesInfo, bi);

					restanteBloque = TAMANIO_BLOQUE;
					i++;

					memset(bloque, '\0', TAMANIO_BLOQUE);
					offsetbloque = 0;
				}

				memcpy(bloque + offsetbloque, contenido + offset, d);

				finbytes += d;

				restanteBloque -= d;
				offsetbloque += d;
				offset += d;
				d = 0;

			}
		}

		/**/

/// *** Copiado
//************************************************
// Decido cual nodo
//************************************************
// Selecciono el siguiente nodo (ya preseleccionado)
		nb = list_get(bl, i);
		bloqueAModificar = nb->idBloque;
		idnodo = nb->idNodo;

		int l = 0;
		t_nodo* nodo;

		for (l = 0; l < list_size(nodos->nodos); l++) {
			t_nodo* n = list_get(nodos->nodos, l);
			if (n->id == idnodo) {
				nodo = n;
				break;
			}
		}

//************************************************
// Envio bloque al datanode
//************************************************

		log_debug(logger,
				"Escribiendo renglon en nro de bloque %d id: %d nodo: %d", i,
				bloqueAModificar, idnodo);

		//log_debug(logger, "Contenido: %s", bloque);

		int dataNode = escribirEnDataNode(bloqueAModificar, bloque,
				nodo->socketNodo, finbytes, logger);

		sem_wait(&semEscritura);

		t_bloqueInfo* bi = malloc(sizeof(t_bloqueInfo));

		log_info(logger, "Escritura en el datanode realizada con exito");

		// Actualizo lista nodos y nodos.bin
		log_info(logger, "Actualizando nodos.bin...");
		// nodo->libre -= 1; // Ya lo hace la reserva de bloques
		actualizarConfigNodoEnBin(nodo);

		bi->finBytes = finbytes;
		bi->idBloque0 = bloqueAModificar;
		bi->idNodo0 = string_itoa(nodo->id);

//*********************************//

		// Copia del bloque
//*********************************//
// Debo hacer el envio al datanode del mismo contenido en el nodo y bloque id indicado por la lista (el siguiente elemento)

// Selecciono el nodo de la copia (ya preseleccionado)
		nb = list_get(bl, i + 1);
		bloqueAModificar = nb->idBloque;
		idnodo = nb->idNodo;

		i++;

		l = 0;

		for (l = 0; l < list_size(nodos->nodos); l++) {
			t_nodo* n = list_get(nodos->nodos, l);
			if (n->id == idnodo) {
				nodo = n;
				break;
			}
		}

		log_debug(logger,
				"Escribiendo renglon (copia) en nro de bloque %d id: %d nodo: %d",
				i, bloqueAModificar, idnodo);

		//log_debug(logger, "Contenido (copia): %s", bloque);

		dataNode = escribirEnDataNode(bloqueAModificar, bloque,
				nodo->socketNodo, finbytes, logger);

		sem_wait(&semEscritura);

		log_info(logger, "Escritura en el datanode realizada con exito");

		// Actualizo lista nodos y nodos.bin
		log_info(logger, "Actualizando nodos.bin...");
		// nodo->libre -= 1; // Ya lo hace la reserva de bloques
		actualizarConfigNodoEnBin(nodo);

		bi->idBloque1 = bloqueAModificar;
		bi->idNodo1 = string_itoa(nodo->id);

		list_add(bloquesInfo, bi);

	}

	/// *** Tipo de archivo binario

	if (tipo == BINARIO) {

		int i = 0; // Comienzo desde el bloque 0

		t_idNodoBloque* nb = list_get(bl, i);
		int bloqueAModificar = nb->idBloque;
		int idnodo = nb->idNodo;
		int restante = tamanio;

		long finbytes = TAMANIO_BLOQUE; // Para el ultimo, es lo que quede

		for (i = 0; i < bloquesNecesarios * 2; i++) { // El ultimo tiene fragmentacion interna, controlar que se escriba lo ultimo

			// Si es el unico bloque menor al tamanio de bloque O si es el ultimo bloque (que tiene fragmentacion)

			if (restante < TAMANIO_BLOQUE || tamanio < TAMANIO_BLOQUE) {

				int rest = 0;
				if (restante < TAMANIO_BLOQUE)
					rest = restante;
				if (tamanio < TAMANIO_BLOQUE)
					rest = tamanio;

				memcpy(bloque, contenido + offset, rest);

				offset += rest;
				tamanioArchivo += rest;

				finbytes = rest;

			} else {
				memcpy(bloque, contenido + offset, TAMANIO_BLOQUE);

				offset += TAMANIO_BLOQUE;
				tamanioArchivo += TAMANIO_BLOQUE;
				restante -= TAMANIO_BLOQUE;

			}

			// Pedido al datanode

			log_info(logger, "Escribiendo en bloque %d", i);

			/// *** Copiado

			//************************************************
			// Decido cual nodo
			//************************************************

			// Selecciono el siguiente nodo (ya preseleccionado)
			nb = list_get(bl, i);
			bloqueAModificar = nb->idBloque;
			idnodo = nb->idNodo;

			int l = 0;
			t_nodo* nodo;

			for (l = 0; l < list_size(nodos->nodos); l++) {
				t_nodo* n = list_get(nodos->nodos, l);
				if (n->id == idnodo) {
					nodo = n;
					break;
				}
			}

			//************************************************
			// Envio bloque al datanode
			//************************************************

			log_debug(logger,
					"Escribiendo bloque en nro de bloque %d id: %d nodo: %d", i,
					bloqueAModificar, idnodo);

			//log_debug(logger, "Contenido: %s", bloque);

			int dataNode = escribirEnDataNode(bloqueAModificar, bloque,
					nodo->socketNodo, finbytes, logger);

			sem_wait(&semEscritura);

			t_bloqueInfo* bi = malloc(sizeof(t_bloqueInfo));

			log_info(logger, "Escritura en el datanode realizada con exito");

			// Actualizo lista nodos y nodos.bin
			log_info(logger, "Actualizando nodos.bin...");
			// nodo->libre -= 1; // Ya lo hace la reserva de bloques
			actualizarConfigNodoEnBin(nodo);

			bi->finBytes = finbytes;
			//finbytes = 0;
			bi->idBloque0 = bloqueAModificar;
			bi->idNodo0 = string_itoa(nodo->id);

			//*********************************//
			// Copia del bloque
			//*********************************//
			// Debo hacer el envio al datanode del mismo contenido en el nodo y bloque id indicado por la lista (el siguiente elemento)

			// Selecciono el nodo de la copia (ya preseleccionado)
			nb = list_get(bl, i + 1);
			bloqueAModificar = nb->idBloque;
			idnodo = nb->idNodo;

			i++;

			l = 0;

			for (l = 0; l < list_size(nodos->nodos); l++) {
				t_nodo* n = list_get(nodos->nodos, l);
				if (n->id == idnodo) {
					nodo = n;
					break;
				}
			}

			log_debug(logger,
					"Escribiendo renglon (copia) en nro de bloque %d id: %d nodo: %d",
					i, bloqueAModificar, idnodo);

			//log_debug(logger, "Contenido (copia): %s", bloque);

			dataNode = escribirEnDataNode(bloqueAModificar, bloque,
					nodo->socketNodo, finbytes, logger);

			sem_wait(&semEscritura);

			log_info(logger, "Escritura en el datanode realizada con exito");

			// Actualizo lista nodos y nodos.bin
			log_info(logger, "Actualizando nodos.bin...");
			// nodo->libre -= 1; // Ya lo hace la reserva de bloques
			actualizarConfigNodoEnBin(nodo);

			bi->idBloque1 = bloqueAModificar;
			bi->idNodo1 = string_itoa(nodo->id);

			list_add(bloquesInfo, bi);

			memset(bloque, '\0', TAMANIO_BLOQUE);

		}
	}

	log_info(logger,
			"Creando y actualizando estructuras adminsitrativas (metadata de archivo, nodo y bloques)");

// Si se escribieron bloques, genero el archivo de metadata
	if (list_size(bloquesInfo) > 0)
		crearArchivoMetadata(bloquesInfo, path, tipo, tamanioArchivo);
	else
		log_error(logger, "No existen bloques para escribir");

	actualizarBitMapBloques(bloquesInfo);

	list_destroy(bloquesInfo);

	return 0;

}

// Lee un archivo almacenado en el fs yama
char* leerArchivo(char* path, int* codigoError) {

	t_archivoInfo* archivo = obtenerArchivoInfo(path);

	int reiniciar = 0;

	if (archivo == NULL) {
		// No existe el archivo
		*codigoError = -1;
		return NULL;
	}

	char* respuesta;

	int offset = 0;
	int i = 0;

	t_list* nodosExc = list_create();

	while (1) {

		t_list* nodosALeer = obtenerNodosALeer(archivo->bloques, nodosExc);

		if (nodosALeer == NULL) {
			log_error(logger,
					"No hay nodos conectados para realizar la lectura pedida");
			*codigoError = -2;
			return NULL;
		}

		// Preparo lista t_lectura
		lista = list_create();
		int j = 0;
		for (j = 0; j < list_size(archivo->bloques); j++) {
			t_bloqueInfo* bi = list_get(archivo->bloques, j);
			t_lectura* lect = malloc(sizeof(t_lectura));

			t_nodoSelect* res = list_get(nodosALeer, j);

			lect->nroBloque = bi->nroBloque;
			//lect->idNodo = atoi(bi->idNodo0); // TODO: Siempre busco en la primera copia, debe distribuirse

			lect->idNodo = res->idNodo;
			log_debug(logger, "Nodo seleccionado para leer de id: %d",
					res->idNodo);

			lect->finBytes = bi->finBytes;
			lect->lectFallo = 0;
			lect->lectura = '\0';
			sem_init(&lect->lecturaOk, 0, 0);
			list_add(lista, lect);
		}

		i = 0;
		// Envio peticiones a los diferentes datanodes
		for (i = 0; i < list_size(archivo->bloques); i++) {

			t_bloqueInfo* bi = list_get(archivo->bloques, i);

			sem_wait(&semLista);
			t_lectura* lect = list_get(lista, i);
			t_nodo* nodo = buscarNodoPorId_2(lect->idNodo);
			sem_post(&semLista);

			if (nodo == NULL) {

				// Si los excluidos son todos los de las peticiones (y no estan conectados)->aborto lectura, caso contrario reintento lectura

				log_error(logger,
						"No se pudo hacer la lectura porque uno de los nodos esta caido. Reiniciando lectura con los nodos disponibles (excluyendo el nodo %d)...",
						lect->idNodo);
				list_add(nodosExc, lect->idNodo);
				reiniciar = 1;
				break;
			}

			int lectura = leerDeDataNode(bi->idBloque0, nodo->socketNodo,
					bi->finBytes, bi->nroBloque, logger);

			if (lectura == -2) {
				log_error(logger,
						"Hubo un error leyendo los bloques del archivo (posiblemente algun datanode caido)");
				*codigoError = -2;
				return NULL;
			}

		}

		// Debo reiniciar?
		if (reiniciar == 1) {
			reiniciar = 0;
			continue;
		}

		// Espero resultados
		log_info(logger, "Esperando resultado de lectura de los nodos...");
		for (i = 0; i < list_size(archivo->bloques); i++) {
			t_lectura* lect = list_get(lista, i);
			sem_wait(&lect->lecturaOk);
		}

		// Chequeo si hubo alguna falla durante la conexion con nodos
		// TODO: Si hubo, reseteo la lectura con otra estrategia de nodos

		int z = 0;
		sem_wait(&semLista);
		for (z = 0; z < list_size(lista); z++) {
			t_lectura* lect = list_get(lista, z);

			if (lect->lectFallo == 1) {

				log_error(logger,
						"Error en la lectura, algun nodo se desconecto, reseteando lectura con otra estrategia de nodos");
				*codigoError = -2;
				sem_post(&semLista);
				return NULL;

			}

		}
		sem_post(&semLista);

		// Corto el while para mergear resultados
		break;
	}

	log_info(logger,
			"Se recibio la respuesta de todos los nodos involucrados, armando resultado final.");

	// Armo respuesta final

	sem_wait(&semLista);

	respuesta = malloc(archivo->tamanio);

	for (i = 0; i < list_size(archivo->bloques); i++) {
		t_lectura* lect = list_get(lista, i);
		memcpy(respuesta + offset, lect->lectura, lect->finBytes);
		offset += lect->finBytes;
	}

	list_destroy_and_destroy_elements(lista, eliminarItemLectura);

	sem_post(&semLista);

	lista = NULL;
	list_destroy(nodosExc);
	free(archivo);

	return respuesta;
}

void* eliminarItemLectura(t_lectura* lect) {
	free(lect->lectura);
	free(lect);
	return 0;
}

// Obtiene la lista de nodos a leer optima. Excluye los nodos de la lista nodosExcluir
// TODO: chequear caso que no tenga una copia (porque se le borro con rm)
t_list* obtenerNodosALeer(t_list* bloques, t_list* nodosExcluir) {

	t_list* resultado = list_create();
	t_list* nodosSinRepetir = list_create();

	int j = 0;
	j = 0;
	// Obtengo los nodos sin repetir sin los excluidos y comparo
	for (j = 0; j < list_size(bloques); j++) {
		t_bloqueInfo* bi = list_get(bloques, j);

		t_nodoSelect_* ns = malloc(sizeof(t_nodoSelect_));
		ns->idNodo = atoi(bi->idNodo0);
		ns->uso = 0;

		int l = 0;
		int existe = 0;

		for (l = 0; l < list_size(nodosSinRepetir); l++) {
			t_nodo* nsr = list_get(nodosSinRepetir, l);
			if (nsr->id == ns->idNodo) {
				existe = 1;
				break;
			}
		}

		if (existe == 0)
			list_add(nodosSinRepetir, ns);

		ns = malloc(sizeof(t_nodoSelect_));
		ns->idNodo = atoi(bi->idNodo1);
		ns->uso = 0;

		l = 0;
		existe = 0;
		for (l = 0; l < list_size(nodosSinRepetir); l++) {
			t_nodoSelect_* nsr = list_get(nodosSinRepetir, l);
			if (nsr->idNodo == ns->idNodo) {
				existe = 1;
				break;
			}
		}

		if (existe == 0)
			list_add(nodosSinRepetir, ns);

	}

	log_debug(logger, "Nodos sin repetir: %d - nodos a excluir: %d",
			list_size(nodosSinRepetir), list_size(nodosExcluir));

	/*	if (list_size(nodosSinRepetir) == list_size(nodosExcluir)) {
	 list_destroy(nodosSinRepetir);
	 return NULL;
	 }*/

	list_clean(nodosSinRepetir);

	// Creo lista de resultados
	j = 0;
	for (j = 0; j < list_size(bloques); j++) {
		t_bloqueInfo* bi = list_get(bloques, j);
		t_nodoSelect* ns = malloc(sizeof(t_nodoSelect));
		ns->nroBloque = bi->nroBloque;
		ns->nodo1 = atoi(bi->idNodo0);
		ns->nodo2 = atoi(bi->idNodo1);
		list_add(resultado, ns);
	}

	// Creo lista de nodos involucrados sin repetir

	j = 0;
	for (j = 0; j < list_size(bloques); j++) {
		t_bloqueInfo* bi = list_get(bloques, j);

		t_nodoSelect_* ns = malloc(sizeof(t_nodoSelect_));
		ns->idNodo = atoi(bi->idNodo0);
		ns->uso = 0;

		int l = 0;
		int existe = 0;
		int excluir = 0;

		for (l = 0; l < list_size(nodosExcluir); l++) {
			int nsr = list_get(nodosExcluir, l);
			if (nsr == ns->idNodo) {
				excluir = 1;
				break;
			}
		}

		existe = 0;
		l = 0;
		for (l = 0; l < list_size(nodosSinRepetir); l++) {
			t_nodo* nsr = list_get(nodosSinRepetir, l);
			if (nsr->id == ns->idNodo) {
				existe = 1;
				break;
			}
		}

		if (existe == 0 && excluir == 0)
			list_add(nodosSinRepetir, ns);

		ns = malloc(sizeof(t_nodoSelect_));
		ns->idNodo = atoi(bi->idNodo1);
		ns->uso = 0;

		excluir = 0;

		for (l = 0; l < list_size(nodosExcluir); l++) {
			int nsr = list_get(nodosExcluir, l);
			if (nsr == ns->idNodo) {
				excluir = 1;
				break;
			}
		}

		l = 0;
		existe = 0;
		for (l = 0; l < list_size(nodosSinRepetir); l++) {
			t_nodoSelect_* nsr = list_get(nodosSinRepetir, l);
			if (nsr->idNodo == ns->idNodo) {
				existe = 1;
				break;
			}
		}

		if (existe == 0 && excluir == 0)
			list_add(nodosSinRepetir, ns);

	}

	int i = 0;
	int k = 0;

	int encontro = 0;
	int iteraciones=0;

	// Selecciono nodos
	for (i = 0; i < list_size(resultado); i++) {
		encontro = 0;
		iteraciones++;
		if (iteraciones>list_size(resultado)*2) {
			log_debug(logger,"Iteraciones: %d", iteraciones);
			return NULL;
		}

		for (k = 0; k < list_size(nodosSinRepetir); k++) {
			t_nodoSelect_* nid = list_get(nodosSinRepetir, k);
			t_nodoSelect* res = list_get(resultado, i);
			if (nid->uso == 0
					&& (res->nodo1 == nid->idNodo || res->nodo2 == nid->idNodo)) {
				nid->uso = 1;
				res->idNodo = nid->idNodo;
				encontro = 1;
				break;
			}
		}


		// Si no encontro nodo sin uso en 0->reseteo todos y vuelvo a buscar
		if (encontro == 0) {
			i--;
			k = 0;
			for (k = 0; k < list_size(nodosSinRepetir); k++) {
				t_nodoSelect_* nid = list_get(nodosSinRepetir, k);
				nid->uso = 0;
			}
		}

	}
	log_debug(logger,"Iteraciones: %d", iteraciones);

	list_destroy(nodosSinRepetir);

	return resultado;

}

// Copia un archivo de yama al fs nativo
int copiarDesdeYamaALocal(char* origen, char* destino) {

	t_archivoInfo* aInfo = obtenerArchivoInfo(origen);
	if (aInfo == NULL) {
		return -1;
	}

	int errorCode = 0;
	char* lectura = leerArchivo(origen, &errorCode);

	switch (errorCode) {
	case -1:
		printf("No existe el archivo\n");
		return -1;
	case -2:
		printf("Algun nodo se desconecto\n");
		return -3;
	}

	int fd, offset;
	struct stat sbuf;

	// Creo el archivo destino en el fs nativo

	char* destinoArchivo;

	char* dest = string_new();
	string_append(&dest, destino);
	string_append(&dest, "/");
	string_append(&dest, obtenerNombreArchivo(origen));

	if ((fd = open(dest, O_RDWR | O_CREAT, 00777)) == -1) {
		log_error(logger, "No se pudo crear el archivo: %s", dest);
		return -2;
	}

	// Trunco con el tamanio obtenido
	ftruncate(fd, aInfo->tamanio);

	// mapeo a memoria
	destinoArchivo = mmap((caddr_t) 0, aInfo->tamanio, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);

	if (destinoArchivo == NULL) {
		perror("error en map\n");
		exit(1);
	}

	// Escribo archivo
	memcpy(destinoArchivo, lectura, aInfo->tamanio);

	return 0;

}
