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

typedef struct {
	int job;
	int master;
	int node;
	int block;
	char op;
	char tmp[28];
	int status;
} rowStateTable;

typedef struct {
	char fs_ip[15];
	int fs_port;
	int fs_socket;
	int planning_delay;
	char balancign_algoritm[3];
	int port;
	t_list* stateTable;
	Server yama_server;
	Client yama_client;
	t_log* log;
} Yama;

/*---------------------- Private ---------------------------------*/
t_config* getConfig(Yama* yama);

t_log* configLog();

void setProperties(Yama* yama, t_config* config);

#endif /* YAMA_H_ */
