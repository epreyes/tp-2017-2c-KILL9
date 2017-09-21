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

t_log *logger;

// Puntero al inicio de la tabla de directorios
t_directorio* inicioTablaDirectorios;

// Tabla de archivos (es una lista de archivos con su path completo empezando desde el indice de directorio padre)
t_list* tablaArchivos;

int main(void) {

	logger = log_create("yamafs.log", "PD_FS", 1, LOG_LEVEL_DEBUG);

	iniciarFS();

	// Esta seccion de codigo por ahora sirve para pruebas

	// Formateo el fs
	formatear(inicioTablaDirectorios);

	crearDirectorio(inicioTablaDirectorios, "test");
	crearDirectorio(inicioTablaDirectorios, "test/test2");

	log_info(logger, "Listando tabla de directorios");
	listarDirectorios(inicioTablaDirectorios);

	t_list* l = list_create();
	log_info(logger, "Listando archivos de test/test2");
	l = listarArchivos("test/test2", inicioTablaDirectorios);

	int j = 0;
	for (j = 0; j < list_size(l); j++)
		printf("%s\n", list_get(l, j));

	// Fin pruebas

	return EXIT_SUCCESS;
}

void iniciarFS() {

	int fd, offset;
	char *data;
	struct stat sbuf;
	//char* archivo="metadata/directorios.dat";
	char* archivo = "/home/utnso/SO/fs/bin/metadata/directorios.dat";

	// TODO: el archivo debe generarse con truncate! sino no puede escribir

	log_info(logger, "Abriendo archivo de directorios %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {
		log_error(logger, "No existe el archivo%s", archivo);
		exit(1);
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	inicioTablaDirectorios = mmap((caddr_t) 0, sbuf.st_size,
	PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);

	if (inicioTablaDirectorios == NULL) {
		perror("error en map\n");
		exit(1);
	}

	log_info(logger, "Archivo de directorios abierto correctamente");

}

