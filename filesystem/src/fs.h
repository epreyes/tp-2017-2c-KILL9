/*
 * fs.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef FS_H_
#define FS_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

// Estructuras administrativas del FS

#define TAMANIO_BLOQUE 6 // Tamanio del bloque en bytes 1MB (1048576 bytes). TODO: Por ahora uso un tamanio chico para pruebas
#define MAX_DIR_FS 100

typedef struct {
	int indice;
	char nombre[255];
	int padre;
} t_directorio;

// Se usan estas estructuras por el momento para gestionar los nodos, debe hacerse por archivo
typedef struct {
	int id;
	int total;
	int libre;
	char* direccion; // TODO: Cuando se cambie a archivo, esto debe construirse en un struct aparte
	int socketNodo; // TODO: Cuando se cambie a archivo, esto debe construirse en un struct aparte
} t_nodo;

typedef struct {
	int tamanio;
	int libre;
	t_list* nodos;
} t_nodos;

// *************** //

// Estructura para tener acceso directo a los bitarray de bloques
typedef struct {
	int idNodo;
	t_bitarray* bitMapBloques;
} t_nodosBitMap;

// Estructura de relacion nodo-bloque para reservar los bloques durante la escritura
typedef struct {
	int idNodo;
	int idBloque;
} t_idNodoBloque;

// Estructura para guardar la informacion de un archivo
typedef struct {
	int tamanio;
	int tipo;
	t_list* bloques;
} t_archivoInfo;

// Puntero al inicio de la tabla de directorios
t_directorio* inicioTablaDirectorios;

// Tabla de archivos (es una lista de archivos con su path completo empezando desde el indice de directorio padre)
t_list* tablaArchivos;

// Estructura que guarda los nodos que van conectandose (TODO: por ahora se persiste en archivo de registros, debe ser de texto)
t_nodos* nodos;

// Lista que guarda punteros (t_nodosBitMap) a los bitmaps de bloques por cada nodo creado al conectarse (para facil acceso al actualizarlos)
t_list* nodosBitMap;

t_log *logger;

// Includes propios

#include "utils.h"
#include "fs_core.h"
#include "fs_servidor.h"


// Tipo de archivo
#define BINARIO 0
#define TEXTO 1

#define RESULTADO_OK 1000

void iniciarFS();

#endif /* FS_H_ */
