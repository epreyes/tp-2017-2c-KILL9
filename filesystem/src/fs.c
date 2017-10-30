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

	sem_init(&semEscritura, 0, 1);

	logger = log_create("yamafs.log", "YAMA_FS", 0, LOG_LEVEL_DEBUG);

	fs = malloc(sizeof(t_fs));

	cargarArchivoDeConfiguracion(fs, "yamafs.cfg");

	iniciarFS();

	nodos = malloc(sizeof(t_nodos));
	nodos->tamanio = 100; // TODO: esta info debe venir de los nodos (una vez que se coenctan)
	nodos->libre = 100;
	nodos->nodos = list_create();

	nodosBitMap = list_create();

	pthread_t hilo_servidor;

	if (pthread_create(&hilo_servidor, NULL, lanzarHiloServidor, (void*) 0)
			< 0) {
		perror("Error creando thread servidor");
		exit(1);
	}

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
