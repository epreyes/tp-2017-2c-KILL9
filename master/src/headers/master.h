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

//===================ANSWERS TAD===============

typedef struct tr_datos tr_datos;
typedef struct rl_datos rl_datos;
typedef struct rg_datos rg_datos;
typedef struct af_datos af_datos;


struct tr_datos{
	int		nodo;
	char	direccion[21];
	int		bloque;
	int		tamanio;
	char	tr_tmp[28];
};

struct rl_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[28];
	char	rl_tmp[28];
};

struct rg_datos{				//es un record por nodo
	int		nodo;
	char	direccion[21];
	char	rl_tmp[28];
	char	rg_tmp[24];
	char	encargado;
};

struct af_datos{
	int		nodo;
	char	direccion[21];
	char	rg_tmp[24];
};

#endif /* MASTER_H_ */
