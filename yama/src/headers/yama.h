/*
 * yama.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <tplibraries/utils/utils.h>
#include <tplibraries/sockets/socket.h>
#include <tplibraries/protocol/master_yama.h>
#include <tplibraries/protocol/filesystem_yama.h>
#include <errno.h>

#include "messages.h"

static char* CONFIG_PATH = "./config/yamaConfig.properties";
static char* LOG_PATH = "./log/yama.log";

typedef struct {
	int blocks;
	int sizeInfo;
	char filename[28];
	void* info;
} elem_info_archivo;

typedef struct {
	int node_id;
	unsigned int tasks_in_progress;
	int tasts_done;
	int considered;
	int availability;
	int errors;
} elem_tabla_nodos;

typedef struct {
	int job;
	int master;
	int node;
	int node_block;
	int block;
	char op;
	char status;
	char tmp[28];
} elem_tabla_estados;

typedef struct{
	tr_datos* data;
	int master;
}elem_tabla_planificados;

typedef struct{
	rl_datos* data;
	int master;
}elem_tabla_LR_planificados;

typedef struct{
	rg_datos* data;
	int master;
}elem_tabla_GR_planificados;

typedef struct {
	t_config* config;
	t_list* tabla_nodos;
	t_list* tabla_estados;
	t_list* tabla_info_archivos;
	t_list* tabla_T_planificados;
	t_list* tabla_LR_planificados;
	t_list* tabla_GR_planificados;
	Server yama_server;
	Client yama_client;
	t_log* log;
	int jobs;
	int clock;
	int clock_aux;
	int debug;
} Yama;

Yama* yama;

/*---------------------- Private ---------------------------------*/
t_config* getConfig();

t_log* getLog();

/*---------------------- Public ----------------------------------*/
void init();

void* getResponse(int master, char request);

void viewConfig();
/*
 * Proceso la operacion que viene en el header. Si es una transformacion, saco la info de la tabla de archivos, si existe;
 * si no, se la pido al filesystem.
 * */
void* processOperation(int master, char op);

#endif /* YAMA_H_ */
