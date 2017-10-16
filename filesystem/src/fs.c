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

	// Simulo conexion de nodos

	nodos = malloc(sizeof(t_nodos));
	nodos->tamanio = 100;
	nodos->libre = 100;
	nodos->nodos = list_create();

	t_nodo* nodo1 = malloc(sizeof(t_nodo));
	nodo1->id = 1;
	nodo1->libre = 8;
	nodo1->total = 8;
	nodo1->direccion = "127.0.0.1:6001";

	t_nodo* nodo2 = malloc(sizeof(t_nodo));
	nodo2->id = 2;
	nodo2->libre = 8;
	nodo2->total = 8;
	nodo2->direccion = "127.0.0.2:6002";

	t_nodo* nodo3 = malloc(sizeof(t_nodo));
	nodo3->id = 5;
	nodo3->libre = 8;
	nodo3->total = 8;
	nodo3->direccion = "127.0.0.2:6003";

	nodos->nodos = list_create();

	list_add(nodos->nodos, nodo1);
	//list_add(nodos->nodos, nodo2);

	nodosBitMap = list_create();

	crearBitMapBloquesNodo(nodo1);
	//crearBitMapBloquesNodo(nodo2);

	/*	log_info(logger, "---existeArchivo test/test2/test2.csv test---");

	 int t = existeArchivo("test/test2/test2");

	 log_info(logger, "Existe: %d", t);

	 log_info(logger, "---existeArchivo enRaiz.csv test---");

	 t = existeArchivo("enRaiz");

	 log_info(logger, "Existe: %d", t);*/

	log_info(logger, "---escribirArchivo('test/test2/prueba') test---");

	char* contenido = "12345\nA\n";
	int escribir = escribirArchivo("test/test2/prueba", contenido, TEXTO);

	if (escribir == 0)
		log_info(logger, "Escritura de prueba realizada con exito");
	else
		log_error(logger, "No se pudo escribir el archivo prueba (error %d)",
				escribir);

	/*********************************************************************************************/

	log_info(logger, "---obtenerArchivoInfo('test/test2/prueba') test---");

	t_archivoInfo* info = obtenerArchivoInfo("test/test2/prueba");
	int i = 0;
	if (info == NULL)
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
		list_destroy(info->bloques);
	}

	free(info);

	/*********************************************************************************************/

	log_info(logger,
			"*********************************************************************************************");

	// Fin pruebas

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
