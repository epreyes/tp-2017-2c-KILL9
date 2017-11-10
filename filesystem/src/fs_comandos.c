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

// Debe ser *2 para hacer las copias
	t_list* bl = obtenerBloquesLibres(bloquesNecesarios * 2);

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

					log_debug(logger, "Contenido: %s", bloque);

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

					log_debug(logger, "Contenido (copia): %s", bloque);

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

		log_debug(logger, "Contenido: %s", bloque);

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

		log_debug(logger, "Contenido (copia): %s", bloque);

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

			log_info(logger, "Escribiendo en bloque %d: %s", i, bloque);

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

			log_debug(logger, "Contenido: %s", bloque);

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

			log_debug(logger, "Contenido (copia): %s", bloque);

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
char* leerArchivo(char* path) {

	t_archivoInfo* archivo = obtenerArchivoInfo(path);

	if (archivo == NULL) {
		// No existe el archivo
		return NULL;
	}

	// Preparo lista t_lectura
	lista = list_create();
	int j = 0;
	for (j = 0; j < list_size(archivo->bloques); j++) {
		t_bloqueInfo* bi = list_get(archivo->bloques, j);
		t_lectura* lect = malloc(sizeof(t_lectura));
		lect->nroBloque = bi->nroBloque;
		lect->idNodo = atoi(bi->idNodo0); // TODO: Siempre busco en la primera copia, debe distribuirse
		lect->finBytes = bi->finBytes;
		sem_init(&lect->lecturaOk, 0, 0);
		list_add(lista, lect);
	}

	char* respuesta = malloc(archivo->tamanio);
	int offset = 0;

	int i = 0;
	// Envio peticiones a los diferentes datanodes
	for (i = 0; i < list_size(archivo->bloques); i++) {
		t_bloqueInfo* bi = list_get(archivo->bloques, i);
		t_lectura* lect = list_get(lista, i);

		t_nodo* nodo = buscarNodoPorId_(lect->idNodo);

		int lectura = leerDeDataNode(bi->idBloque0, nodo->socketNodo,
				bi->finBytes, bi->nroBloque, logger);

	}

	// Espero resultados
	log_info(logger, "Esperando resultado de lectura de los nodos...");
	for (i = 0; i < list_size(archivo->bloques); i++) {
		t_lectura* lect = list_get(lista, i);
		sem_wait(&lect->lecturaOk);
	}

	log_info(logger,
			"Se recibio la respuesta de todos los nodos involucrados, armando resultado final.");

	// Armo respuesta final
	for (i = 0; i < list_size(archivo->bloques); i++) {
		t_lectura* lect = list_get(lista, i);
		memcpy(respuesta + offset, lect->lectura, lect->finBytes);
		offset += lect->finBytes;

	}

	list_destroy(lista);
	free(archivo);

	return respuesta;
}

// Copia un archivo de yama al fs nativo
int copiarDesdeYamaALocal(char* origen, char* destino) {

	t_archivoInfo* aInfo = obtenerArchivoInfo(origen);
	if (aInfo == NULL) {
		return -1;
	}

	char* lectura = leerArchivo(origen);

	if (lectura == NULL)
		return -1;

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
