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
#include <tplibraries/protocol/master_worker.h>
#include <unistd.h>

//----GLOBALS--
FILE* transformScript; 	//lo tomo del pedido de transformación
FILE* reductionScript;	//lo tomo del pedido de reducción local

#endif /* HEADERS_WORKER_H_ */
