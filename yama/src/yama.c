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
		perror("Missing configuration file.");
		exit(1);
	}
	return config;
}

t_log* getLog() {
	return log_create(LOG_PATH, "YAMA", 1, LOG_LEVEL_TRACE);
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
	yama->tabla_planificados = list_create();

	/* inicializo el server en el socket configurado */
	yama->yama_server = startServer(
			config_get_int_value(yama->config, "YAMA_PUERTO"));

	log_trace(yama->log, "YAMA was succesfully configured.");
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
		response = processTransformation(master);

		break;
	case 'L':
		//response = processLocalReduction(yama, head, master);
		break;
	case 'G':
		//response = processGlobalReduction(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
		break;
	case 'S':
		//response = processFinalStore(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
		break;
	case 'E':
		//response = processError(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
		break;
	}
	return response;
}

/************************* Utils **********************************/
/******************************************************************/
void getTmpName(tr_datos* nodeData, int op, int blockId, int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);

	strcpy(nodeData->tr_tmp, name);
}

