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

#include <semaphore.h>

// Estructuras administrativas del FS

#define TAMANIO_BLOQUE 1000000 // Tamanio del bloque en bytes 1MB (1048576 bytes). TODO: Por ahora uso un tamanio chico para pruebas
#define MAX_DIR_FS 100

typedef struct {
	int indice;
	char nombre[255];
	int padre;
} t_directorio;


typedef struct {
	int id;
	int total;
	int libre;
	char* direccion; // Direccion del worker
	int socketNodo;
	int activo; // Para eliminacion logica
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

// **** Estructuras de inicializacion ****

// Estructura usada para mantener el estado de cada bloque de un archivo
typedef struct {
	int nroBloque;   // nro del bloque del archivo (no se refiere a instancia)
	sem_t semaforo; // Se usa para desbloquear en inicializacion una vez que se obtuvo una instancia del bloque en la conexion del nodo
	int cantInstancias; // Cantidad de instancias del bloque del archivo
} t_bloqueInit;

// Estructura usada para mantener la informacion de los bloques de un archivo en la inicializacion
typedef struct {
	char* identificador; // Es el path completo del archivo de metadata
	// Lista de archivos del fs que se genera en el proceso de inicializacion (si se recupera de un estado anterior o no)
	// Se utiliza para saber si un archivo esta disponible o no
	t_list* bloques; // Lista de t_bloqueInit
} t_archivoInit;

// Tabla de archivos
t_list* tablaArchivos; // Lista de t_archivoInit

// **** Fin estructuras de inicializacion ***

// Estructura usada para lectura en paralelo con los nodos
typedef struct {
	int nroBloque;
	int idNodo;
	sem_t lecturaOk;
	int finBytes;
	char* lectura;
	int lectFallo;
} t_lectura;

// Estructura usada para armar el resultado de lectura
typedef struct {
	int nroBloque;
	int idNodo; // nodo a leer
	int nodo1;
	int nodo2;
} t_nodoSelect;

// Estructura usada para seleccionar el nodo a leer
typedef struct {
	int idNodo;
	int uso;
} t_nodoSelect_;


// Puntero al inicio de la tabla de directorios
t_directorio* inicioTablaDirectorios;

// Estructura que guarda los nodos que van conectandose (TODO: por ahora se persiste en archivo de registros, debe ser de texto)
t_nodos* nodos;

// Lista que guarda punteros (t_nodosBitMap) a los bitmaps de bloques por cada nodo creado al conectarse (para facil acceso al actualizarlos)
t_list* nodosBitMap;

t_log *logger;

typedef struct {
	t_config * config;
	int32_t puerto;
	char* m_directorios; // path a archivo directorios.dat
	char* m_nodos;		 // path a archivo nodos.bin
	char* m_bitmap;      // path a directorio donde guardar los bitmaps
	char* m_archivos; // path a directorio donde guardar la metadata de los archivos generados
} t_fs;

t_fs *fs;

// Includes propios

#include "utils.h"
//#include "fs_core.h" // es necesario?
#include "fs_servidor.h"
#include "fs_consola.h"

// Tipo de archivo
#define BINARIO 0
#define TEXTO 1

#define RESULTADO_OK 1000

void iniciarFS();
void cargarArchivoDeConfiguracion(t_fs *fs, char *configPath);

void obtenerNodosUnaInstanciaArchivos();
void esperarConexionNodos();

void iniciarSemaforos();

// Semaforo para escritura desde consola
sem_t semEscritura;

// Lista con informacion de gestion para lectura en paralelo con los nodos
t_list* lista;
sem_t semLista; // Semaforo para exclusion mutua de la lista de lectura

// Semaforos para conexion de nodos (sin estado previo)
sem_t nodoInit;

// Semaforos para exclusion mutua en lista de nodos
sem_t listaNodos;

// Semaforos para exclusion mutua en tabla de archivos
sem_t semTablaArchivos;

// Determina si no hay estado previo
int vieneDeNoEstado;

// Estado del fs
int estadoEstable;

#endif /* FS_H_ */
