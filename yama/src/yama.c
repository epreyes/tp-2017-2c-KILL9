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
	yama->tabla_jobs = list_create();
	yama->tabla_info_archivos = list_create();
	yama->tabla_T_planificados = list_create();
	yama->tabla_LR_planificados = list_create();
	yama->tabla_GR_planificados = list_create();
	yama->availBase = config_get_int_value(yama->config, "NODE_AVAIL");
	yama->planningDelay = config_get_int_value(yama->config,
			"RETARDO_PLANIFICACION");
	strcpy(yama->algoritm,
			config_get_string_value(yama->config, "ALGORITMO_BALANCEO"));

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

int getJobIndex(int master, char etapa) {
	int index = 0;
	if (etapa == 'R') {
		etapa = 'T';
	}
	for (index = 0; index < list_size(yama->tabla_jobs); index++) {
		t_job* job = list_get(yama->tabla_jobs, index);
		if ((job->master == master) && (job->etapa == etapa)
				&& (job->estado == 'P')) {
			return index;
		}
	}
	return -1;
}

int existJob(int jobid, int master, char op) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_jobs); index++) {
		t_job* job = list_get(yama->tabla_jobs, index);
		if ((job->id == jobid) && (job->master == master)
				&& (job->etapa == op)) {
			return index;
		}
	}
	return -1;
}

void abortInProcessJobs(int master) {
	//aborto el job para el master que se desconecto, si es que tenia pendientes.
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_jobs); index++) {
		t_job* job = list_get(yama->tabla_jobs, index);
		if ((job->master == master) && (job->estado == 'P')) {
			job->estado = 'E';
			list_replace(yama->tabla_jobs, index, job);

			int j = 0;
			for(j = 0; j < list_size(yama->tabla_estados); j++){
				elem_tabla_estados* elem = list_get(yama->tabla_estados, j);
				if( elem->master == master && elem->status == 'P'){
					elem->status = 'E';
					list_replace(yama->tabla_estados, j, elem);
				}
			}
		}
	}
}

/*
 * Proceso la operacion que viene en el header. Si es una transformacion, saco la info de la tabla de archivos, si existe;
 * si no, se la pido al filesystem.
 * */
void* processOperation(int master, char op) {
	void* response = NULL;

	switch (op) {
	case 'T': {
		log_trace(yama->log, "Solicitud de transformación. Inicia el Job %d.",
				yama->jobs);

		if (existJob(yama->jobs, master, op) > -1) {
			char msg[] =
					"No se pueden ejecutar dos transformaciones provenientes de un mismo Master.";
			int len = strlen(msg);
			response = malloc(sizeof(char) + sizeof(int) + len);
			memcpy(response, "D", sizeof(char));
			memcpy(response + sizeof(char), &len, sizeof(int));
			memcpy(response + sizeof(char) + sizeof(int), msg, len);
		} else {
			t_job* job = malloc(sizeof(t_job));
			job->estado = 'P';
			job->etapa = 'T';
			job->id = yama->jobs;
			job->replanificaciones = 0;
			job->master = master;
			list_add(yama->tabla_jobs, job);
			response = processTransformation(master, job);
		}
	}
		break;
	case 'L': {
		int jobIndex = getJobIndex(master, 'T');
		t_job* job = list_get(yama->tabla_jobs, jobIndex);
		log_trace(yama->log, "Solicitud de Reducción Local. Job %d.", job->id);
		response = processLocalReduction(master, job->id);
		job->etapa = 'L';
		list_replace(yama->tabla_jobs, jobIndex, job);
	}
		break;
	case 'G': {
		int jobIndex = getJobIndex(master, 'L');
		t_job* job = list_get(yama->tabla_jobs, jobIndex);
		log_trace(yama->log, "Solicitud de Reducción Global. Job %d", job->id);
		response = processGlobalReduction(master, job->id);
		job->etapa = 'G';
		list_replace(yama->tabla_jobs, jobIndex, job);
	}
		break;
	case 'S': {
		int jobIndex = getJobIndex(master, 'G');
		t_job* job = list_get(yama->tabla_jobs, jobIndex);
		log_trace(yama->log, "Solicitud de Almacenamiento Final. Job %d.",
				job->id);
		response = processFinalStore(master, job->id);
		job->etapa = 'S';
		list_replace(yama->tabla_jobs, jobIndex, job);
	}
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
