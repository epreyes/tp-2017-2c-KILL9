/*
 ============================================================================
 Name        : fs.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "fs.h"

// TODO: parametrizar esto
char* directoriosDat = "/home/proyectomacro/SO/fs/bin/metadata/directorios.dat";
char* nodosBin = "/home/proyectomacro/SO/fs/bin/metadata/nodos.bin";

int debug;

/**
int main(void) {

	debug = 1;

	logger = log_create("yamafs.log", "PD_FS", 1, LOG_LEVEL_DEBUG);

	iniciarFS();

	// Esta seccion de codigo por ahora sirve para pruebas

	// Formateo el fs
	formatear();

	crearDirectorio("test");

	crearDirectorio("test/test2");

	log_info(logger, "Listando tabla de directorios");
	listarDirectorios(inicioTablaDirectorios);

	t_list* l = list_create();
	log_info(logger, "Listando archivos de raiz");
	l = listarArchivos("test");

	int j = 0;
	for (j = 0; j < list_size(l); j++)
		printf("%s\n", list_get(l, j));

	list_destroy(l);

//	simularEntradaConexionNodo(nodos);

	nodos = malloc(sizeof(t_nodos));
	nodos->tamanio = 100;
	nodos->libre = 100;
	nodos->nodos = list_create();

	t_nodo* nodo1 = malloc(sizeof(t_nodo));
	nodo1->id = 3;
	nodo1->libre = 100;
	nodo1->total = 100;

	nodos->nodos = list_create();

	list_add(nodos->nodos, nodo1);

	crearBitMapBloquesNodo(nodo1);

	log_info(logger, "---existeArchivo test/test2/test2.csv test---");

	int t = existeArchivo("test/test2/test2");

	log_info(logger, "Existe: %d", t);

	log_info(logger, "---existeArchivo enRaiz.csv test---");

	t = existeArchivo("enRaiz");

	log_info(logger, "Existe: %d", t);

	log_info(logger, "---obtenerArchivoInfo('test/test2/test2') test---");

	t_archivoInfo* info = obtenerArchivoInfo("test/test2/test2");
	int i = 0;
	if (info == NULL )
		log_error(logger, "No se pudo obtener la info de archivo");
	else {
		log_info(logger, "tamanio archivo: %d", info->tamanio);
		log_info(logger, "tipo: %d", info->tipo);
		for (i = 0; i < list_size(info->bloques); i++) {
			t_bloqueInfo* bi = list_get(info->bloques, i);

			log_info(logger, "Bloque %d copia 0 - nodo: %s - bloqueId: %d", i,
					bi->idNodo0, bi->idBloque0);
			log_info(logger, "Bloque %d copia 1 - nodo: %s - bloqueId: %d", i,
					bi->idNodo1, bi->idBloque1);
			log_info(logger, "Bloque %d fin bytes: %d", i, bi->finBytes);

			free(bi);
		}
	}
	list_destroy(info->bloques);
	free(info);

	/*********************************************************************************************/

	log_info(logger, "---escribirArchivo('test/test2/test2') test---");

	char* contenido = "Esto es una prueba de escritura";
	int escribir = escribirArchivo("test/test2/test2", contenido, BINARIO);

	/*********************************************************************************************/

	// Fin pruebas
	log_destroy(logger);

	return EXIT_SUCCESS;
}
*/
void iniciarFS() {

	int fd;
	struct stat sbuf;
	char* archivo;

	archivo = directoriosDat;

	// TODO: el archivo debe generarse con truncate! sino no puede escribir

	log_info(logger, "Abriendo archivo de directorios %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {
		log_error(logger, "No existe el archivo %s", archivo);
		exit(1);
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	inicioTablaDirectorios = mmap((caddr_t) 0, sbuf.st_size,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (inicioTablaDirectorios == NULL ) {
		perror("error en map\n");
		exit(1);
	}

	log_info(logger, "Archivo de directorios abierto correctamente");

	/// **** TODO: el archivo debe ser de formato que indica el enunciado (no de registros)

	archivo = nodosBin;

	// TODO: el archivo debe generarse con truncate! sino no puede escribir

	log_info(logger, "Abriendo archivo de nodos %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {
		log_error(logger, "No existe el archivo %s", archivo);
		exit(1);
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	nodos = mmap((caddr_t) 0, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED,
			fd, 0);

	if (nodos == NULL ) {
		perror("error en map\n");
		exit(1);
	}

	log_info(logger, "Archivo de nodos abierto correctamente");

}

