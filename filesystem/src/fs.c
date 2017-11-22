/*
 ============================================================================
 Name        : fs.c
 Author      : epreyes
 Version     :
 Copyright   :
 Description : Yama Filesystem
 ============================================================================
 */

#include "fs.h"

int main(void) {

	tablaArchivos = list_create();

	logger = log_create("yamafs.log", "YAMA_FS", 0, LOG_LEVEL_DEBUG);

	fs = malloc(sizeof(t_fs));

	cargarArchivoDeConfiguracion(fs, "yamafs.cfg");

	iniciarSemaforos();

	iniciarFS();

	nodos = malloc(sizeof(t_nodos));
	nodos->nodos = list_create();

	nodosBitMap = list_create();

	pthread_t hilo_servidor;

	if (pthread_create(&hilo_servidor, NULL, lanzarHiloServidor, (void*) 0)
			< 0) {
		perror("Error creando thread servidor");
		exit(1);
	}

	inicializacionConNodos();

	ejecutarConsola();

	log_destroy(logger);

	return EXIT_SUCCESS;
}

void cargarArchivoDeConfiguracion(t_fs *fs, char *configPath) {
	fs->config = NULL;
	fs->config = config_create(configPath);

	if (fs->config == NULL ) {
		printf("No existe el archivo de configuracion\n");
		exit(1);
	}

	if (config_has_property(fs->config, "PUERTO")) {
		fs->puerto = config_get_int_value(fs->config, "PUERTO");
	} else {
		printf("Error leyendo archivo de configuracion\n");
		exit(1);
	}

	if (config_has_property(fs->config, "M_DIRECTORIOS")) {
		fs->m_directorios = config_get_string_value(fs->config,
				"M_DIRECTORIOS");
	} else {
		printf("Error leyendo archivo de configuracion\n");
		exit(1);
	}

	if (config_has_property(fs->config, "M_NODOS")) {
		fs->m_nodos = config_get_string_value(fs->config, "M_NODOS");
	} else {
		printf("Error leyendo archivo de configuracion\n");
		exit(1);
	}

	if (config_has_property(fs->config, "M_BITMAP")) {
		fs->m_bitmap = config_get_string_value(fs->config, "M_BITMAP");
	} else {
		printf("Error leyendo archivo de configuracion\n");
		exit(1);
	}

	if (config_has_property(fs->config, "M_ARCHIVOS")) {
		fs->m_archivos = config_get_string_value(fs->config, "M_ARCHIVOS");
	} else {
		printf("Error leyendo archivo de configuracion\n");
		exit(1);
	}

}

void iniciarFS() {

	int fd;
	struct stat sbuf;
	char* archivo;

	estadoEstable = 0;

	archivo = fs->m_directorios;

	log_info(logger, "Inicializando YamaFS", archivo);

	log_info(logger, "Abriendo archivo de directorios %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {

		log_error(logger, "No se pudo abrir el archivo: %s", archivo);
		printf(
				"No se encontro el archivo de directorios, ejecute dformat o revise la configuracion\n");
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

	log_info(logger, "YamaFS iniciado", archivo);

}

int inicializacionConNodos() {

	int estadoAnterior = 0;

	// Existen archivos actualmente en el fs?

	DIR *d;
	struct dirent *dir;

	vieneDeNoEstado = 0;

	d = opendir(fs->m_archivos);
	if (d) {
		while ((dir = readdir(d)) != 0) {
			if (strcmp(dir->d_name, ".") != 0
					&& strcmp(dir->d_name, "..") != 0) {
				estadoAnterior = 1;
				break;

			}

		}

		closedir(d);
	}

	if (estadoAnterior == 0) {

		log_info(logger,
				"No existe estado anterior, esperando nodos a que se conecten para lograr un estado estable");
		printf(
				"No existe estado anterior, esperando nodos a que se conecten para lograr un estado estable. No se permiten conexiones de yama o workers hasta que termine la inicializacion.\n");

		vieneDeNoEstado = 1;

		int cantNodosEspera = 2; // TODO: parametrizar
		int i = 0;
		for (i = 0; i < cantNodosEspera; i++)
			sem_wait(&nodoInit);

		estadoEstable = 1;

	} else {

		log_info(logger,
				"Existe un estado anterior, esperando que se conecten los nodos correspondientes...");
		printf(
				"Existe un estado anterior, esperando que se conecten los nodos correspondientes...\n");

		// Espero la conexion de nodos que contienen 1 instancia de los bloques de todos los archivo (cualquiera de las dos)

		obtenerNodosUnaInstanciaArchivos();
		esperarConexionNodos();

	}

	log_info(logger, "Se llego a un estado estable");
	printf(
			"Se llego a un estado estable. Ahora pueden conectar yama, workers y los nodos del estado anterior que no hayan conectado todavia\n");

	return 0;

}

// Obtiene la lista de nodos que se requieren como minimo (al menos una instancia de cada bloque) para obtener los archivos del estado anterior

void obtenerNodosUnaInstanciaArchivos() {

	// Recorro cada archivo

	int i = 0;
	t_directorio* dir = inicioTablaDirectorios;

	for (i = 0; i < MAX_DIR_FS; i++) {

		if (dir->padre == -1 && dir->indice != 0) {
			dir++;
			continue;
		}
		t_list* archivos = (t_list*) listarArchivos(dir->nombre);
		if (list_size(archivos) == 0) {
			dir++;
			continue;
		}

		int j = 0;

		for (j = 0; j < list_size(archivos); j++) {

			char* arch = list_get(archivos, j);

			char* pathMetadata = string_new();
			string_append(&pathMetadata, fs->m_archivos);
			string_append(&pathMetadata, string_itoa(dir->indice));
			string_append(&pathMetadata, "/");
			string_append(&pathMetadata, arch); // Ya include la extension de la metadata .csv

			t_archivoInit* archivoInit = malloc(sizeof(t_archivoInit));
			archivoInit->bloques = list_create();
			archivoInit->identificador = malloc(strlen(pathMetadata) + 1);
			memcpy(archivoInit->identificador, pathMetadata,
					strlen(pathMetadata));
			archivoInit->identificador[strlen(pathMetadata)] = '\0';

			t_archivoInfo* archInfo = obtenerArchivoInfoPorMetadata(
					pathMetadata);

			// No deberia pasar
			if (archInfo == NULL ) {
				log_error(logger,
						"No se encontro el archivo de metadata en inicializacion");
				exit(1);
			}

			int k = 0;
			for (k = 0; k < list_size(archInfo->bloques); k++) {

				t_bloqueInfo* bi = list_get(archInfo->bloques, k);

				t_bloqueInit* bli = malloc(sizeof(t_bloqueInit));
				bli->cantInstancias = 0;
				bli->nroBloque = k; // TODO: revisar esto, puede que el nro de bloque no coincidan si se elimina un bloque con rm
				// Quizas el obtener bloque info deberia devolver el ID del bloque del archivo y no el indice!!!!

				sem_init(&bli->semaforo, 0, 0);

				list_add(archivoInit->bloques, bli);
			}

			list_add(tablaArchivos, archivoInit);

		}

		list_destroy(archivos);

		dir++;

	}

}

// Espera a que se conecten los nodos que tienen los bloques de los archivos de un estado anterior
void esperarConexionNodos() {

	int i = 0;
	int j = 0;

	printf("Esperando finalizacion de conexion de nodos...\n");

	for (i = 0; i < list_size(tablaArchivos); i++) {
		t_archivoInit* lb = list_get(tablaArchivos, i);
		for (j = 0; j < list_size(lb->bloques); j++) {
			t_bloqueInit* bli = list_get(lb->bloques, j);
			sem_wait(&bli->semaforo);
		}

	}

	estadoEstable = 1;

	printf("Finalizado\n");

}

void iniciarSemaforos() {
	sem_init(&semEscritura, 0, 1);
	sem_init(&nodoInit, 0, 0);
	sem_init(&listaNodos, 0, 1);
	sem_init(&semTablaArchivos, 0, 1);
	sem_init(&semLista, 0, 1);
}
