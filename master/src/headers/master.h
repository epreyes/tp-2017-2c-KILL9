/*
 * master.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef MASTER_H_
#define MASTER_H_

//=========LIBREARIES==========//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <commons/config.h>
#include <commons/log.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "../../protocol/protocol.h"
//=========LOGGERS=============//

t_log* logger;

//=========CONFIGS============//

t_config* config;

//=========SOCKETS=========//

int masterSocket;

//=========METRICS TAD=========//

typedef struct procMetrics{
	double runTime;
	int errors;
	int parallelTask;
	int tasks;
}procMetrics;


typedef struct metrics{
	double runTime;
	procMetrics transformation;
	procMetrics localReduction;
	procMetrics globalReduction;
	procMetrics finalStorage;
}metrics;


#endif /* MASTER_H_ */
