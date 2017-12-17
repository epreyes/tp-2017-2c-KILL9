/*
 * worker.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_WORKER_H_
#define HEADERS_WORKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <tplibraries/protocol/master_worker.h>
#include <tplibraries/protocol/worker_filesystem.h>
#include <tplibraries/protocol/worker_worker.h>
#include <unistd.h>
#include <commons/config.h>
#include <commons/log.h>
#include <signal.h>
#include "utils.h"

//Tamaño de bloque = 1MB
#define BLOCK_SIZE 1000000		//cambiar por 1MB = 1048576


/** variables globales**/
void * mapped_data_node;
size_t mapsize;
//=========LOGGERS=============//
t_log* logger;
//=========CONFIGS============//
t_config* config;
//=========FILES============//
FILE* script_transform;
FILE* script_reduction;
//=========SOCKETS============//
int socket_worker;
int socket_master;
int socket_filesystem;
int socket_nodes[50];

//========PIDS===============//
int pids[10];
#endif /* HEADERS_WORKER_H_ */
