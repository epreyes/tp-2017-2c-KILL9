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
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <tplibraries/sockets/socket.h>
#include <tplibraries/utils/utils.h>
#include <tplibraries/protocol/protocol.h>

static char* CONFIG_PATH = "./config/yamaConfig.properties";
static char* LOG_PATH = "./log/yama.log";

typedef struct{
	int blocks;
	int sizeInfo;
	char* filename;
	void* info;
}t_fileInfo;

typedef struct{
	int node_id;
	int tasks_in_progress;
	int tasts_done;
	int considered;
	int availability;
}t_nodeStateTable;

typedef struct {
	int job;
	int master;
	int node;
	int block;
	char op;
	char tmp[28];
	int status;
} rowStateTable;

/*FS_IP=127.0.1.1
FS_PUERTO=8880
RETARDO_PLANIFICACION=5
ALGORITMO_BALANCEO=WRR
YAMA_PUERTO=8881
NODE_AVAIL = 5*/

typedef struct {
	t_config* config;
	t_list* node_state_table;
	t_list* state_table;
	t_list* file_info;
	Server yama_server;
	Client yama_client;
	t_log* log;
} Yama;

t_config* getConfig();

t_log* configLog();

void setProperties(Yama* yama);

/*---------------------- Public ----------------------------------*/
Yama configYama();

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList, int hightSd);

t_package* getRequest(Yama* yama, void* request, int master);

void* getResponse(Yama* yama, t_package* request, int master);

int sendResponse(Yama* yama, int master, void* masterRS);

int getMasterMessage(Yama* yama, int socket, fd_set* mastersList);

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp, fd_set* mastersList);

void waitForMasters(Yama* yama);

#endif /* YAMA_H_ */
