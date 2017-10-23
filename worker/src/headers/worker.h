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
#include <unistd.h>
#include <commons/config.h>
#include <commons/log.h>
#include "utils.h"

//=========LOGGERS=============//
t_log* logger;
//=========CONFIGS============//
t_config* config;
//=========FILES============//
FILE* script_transform; 	//lo tomo del pedido de transformación
FILE* script_reduction;		//lo tomo del pedido de reducción local
//=========SOCKETS============//
int socket_worker;
int socket_master;

#endif /* HEADERS_WORKER_H_ */
