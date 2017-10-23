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
char* directoriosDat = "metadata/directorios.dat";
char* nodosBin = "metadata/nodos.bin";

int debug;

int main(void) {

	debug = 1;

	logger = log_create("yamafs.log", "YAMA_FS", 1, LOG_LEVEL_DEBUG);

	iniciarFS();

	// Formateo el fs (luego debe sacarse este formateo inicial)
	formatear();

	nodos = malloc(sizeof(t_nodos));
	nodos->tamanio = 100;
	nodos->libre = 100;
	nodos->nodos = list_create();

	nodosBitMap = list_create();

	lanzarHiloServidor();

	log_destroy(logger);

	return EXIT_SUCCESS;
}

void iniciarFS() {

	int fd;
	struct stat sbuf;
	char* archivo;

	archivo = directoriosDat;

	log_info(logger, "Inicializando YamaFS", archivo);

	log_info(logger, "Abriendo archivo de directorios %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {

		log_error(logger, "No se pudo abrir el archivo: %s", archivo);
		exit(1);

	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	inicioTablaDirectorios = mmap((caddr_t) 0, sbuf.st_size,
	PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (inicioTablaDirectorios == NULL) {
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

	if (nodos == NULL) {
		perror("error en map\n");
		exit(1);
	}

	log_info(logger, "Archivo de nodos abierto correctamente");

	log_info(logger, "YamaFS iniciado", archivo);

}
