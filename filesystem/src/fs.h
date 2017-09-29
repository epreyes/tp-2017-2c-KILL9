/*
 * fs.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef FS_H_
#define FS_H_

#include <arpa/inet.h>
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

#define TAMANIO_BLOQUE 16 // Tamanio del bloque en bytes 1MB (1048576 bytes). TODO: Por ahora uso un tamanio chico para pruebas
#define MAX_DIR_FS 100

// Estructura para administrar los archivos del fs (cada archivo tiene un metadata en formato csv)
typedef struct {
	int indice;
	int tamanio;
	int tipo;
	int dirPadre;
	short int estado;
	t_list* bloques;
} t_archivo;

typedef struct {
	;
} t_bloque;

typedef struct {
	int indice;
	char nombre[255];
	int padre;
} t_directorio;

typedef struct {
	int id;
	int total;
	int libre;
} t_nodo;

typedef struct {
	int tamanio;
	int libre;
	t_list* nodos;
} t_nodos;

typedef struct {
	int idNodo;
	t_bitarray bloques;
} t_bitMapBloquesNodo;

typedef struct {
	char* idNodo0;
	char* idNodo1;
	int idBloque0;
	int idBloque1;
	int finBytes;
} t_bloqueInfo;

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

t_log *logger;

// Includes propios

#include "utils.h"
#include "fs_core.h"

// Tipo de archivo
#define BINARIO 0
#define TEXTO 1

void iniciarFS();

#endif /* FS_H_ */
