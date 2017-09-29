/*
 * master.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef MASTER_H_
#define MASTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
//#include <commons/config.h>
//#include <commons/log.h>
#include "answers.h"
#include <time.h>
#include <sys/time.h>


//static char* CONFIG_PATH=".masterConfig.properties";
//static char* LOG_PATH="./logs/master.log";

//=======Estructuras para las métricas=======//
typedef struct procMetrics{
	double runTime;
	int errors;
	int parallelTask;
	int tasks;
}procMetrics;


typedef struct metrics{
	double runTime;
	procMetrics* transformation;
	procMetrics* localReduction;
	procMetrics* globalReduction;
}metrics;

#endif /* MASTER_H_ */
