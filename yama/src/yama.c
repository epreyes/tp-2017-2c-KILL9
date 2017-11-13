/*
 ============================================================================
 Name        : yama.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "headers/yama.h"

/*---------------------- Private ---------------------------------*/
t_config* getConfig() {
	t_config* config;
	if (fileExist(CONFIG_PATH)) {
		config = config_create(CONFIG_PATH);
	} else {
		log_error(yama->log, "Missing configuration File.");
		printf(
				"\nMissing configuration file. Add a configuration file and reload it using SIGURS1 signal.\n");
	}
	return config;
}

t_log* getLog() {
	return log_create(LOG_PATH, "YAMA", 1, LOG_LEVEL_TRACE);
}

void viewConfig() {
	printf(
			"\nCONFIGURACION:\n\nPS_IP: %s\n FS_PUERTO: %d\n RETARDO_PLANIFICACION: %d\n ALGORITMO_BALANCEO: %s\n YAMA_PUERTO: %d\n NODE_AVAIL: %d\n",
			config_get_string_value(yama->config, "FS_IP"),
			config_get_int_value(yama->config, "FS_PUERTO"),
			config_get_int_value(yama->config, "RETARDO_PLANIFICACION"),
			config_get_string_value(yama->config, "ALGORITMO_BALANCEO"),
			config_get_int_value(yama->config, "YAMA_PUERTO"),
			config_get_int_value(yama->config, "NODE_AVAIL"));
}

/*---------------------- Public ----------------------------------*/
void init() {
	yama = malloc(sizeof(Yama));

	/* obtengo el archivo de configuracion */
	yama->config = getConfig();

	/* obtengo el log */
	yama->log = getLog();

	/* inicializo las tablas de informacion necesarias para el procesamiento*/
	yama->tabla_estados = list_create();
	yama->tabla_nodos = list_create();
	yama->tabla_info_archivos = list_create();
	yama->tabla_T_planificados = list_create();
	yama->tabla_LR_planificados = list_create();
	yama->tabla_GR_planificados = list_create();
	yama->availBase = config_get_int_value(yama->config, "NODE_AVAIL");
	yama->planningDelay = config_get_int_value(yama->config, "RETARDO_PLANIFICACION");
	strcpy(yama->algoritm, config_get_string_value(yama->config, "ALGORITMO_BALANCEO"));

	yama->debug = config_get_int_value(yama->config, "DEBUG");

	/* inicializo el server en el socket configurado */
	yama->yama_server = startServer(
			config_get_int_value(yama->config, "YAMA_PUERTO"));

	log_trace(yama->log, "YAMA fue configurado correctamente.");
	yama->jobs = 0;
}

void* getResponse(int master, char request) {
	return processOperation(master, request);
}

/*
 * Proceso la operacion que viene en el header. Si es una transformacion, saco la info de la tabla de archivos, si existe;
 * si no, se la pido al filesystem.
 * */
void* processOperation(int master, char op) {
	void* response = NULL;

	switch (op) {
	case 'T':
		log_trace(yama->log, "Solicitud de transformación. Job %d.",
				yama->jobs + master);
		response = processTransformation(master);
		break;
	case 'L':
		log_trace(yama->log, "Solicitud de Reducción Local. Job %d.",
				yama->jobs + master);
		response = processLocalReduction(master);
		break;
	case 'G':
		log_trace(yama->log, "Solicitud de Reducción Global. Job %d",
				yama->jobs + master);
		response = processGlobalReduction(master);
		break;
	case 'S':
		log_trace(yama->log, "Solicitud de Almacenamiento Final. Job %d.",
				yama->jobs + master);
		response = processFinalStore(master);
		break;
	case 'E':
		response = processNodeError(master);
		break;
	case 'O':
		response = processOk(master);
		break;
	default:
		response = invalidRequest(master, "Error: Invalid operation.");
	}

	viewStateTable();

	return response;
}
