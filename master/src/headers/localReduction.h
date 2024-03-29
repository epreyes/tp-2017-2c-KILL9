/*
 * localReduction.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef HEADERS_LOCALREDUCTION_H_
#define HEADERS_LOCALREDUCTION_H_

#include "master.h"
#include "utils.h"

typedef char tr_tmp[28];

typedef struct dataThread_LR{
	int				node;
	char 			ip[16];
	int				port;
	tr_tmp* 		tr_tmps;
	char			rl_tmp[28];
	int 			tmpsCounter;
	threadMetrics	metrics;
}dataThread_LR;

local_rs* sendLRequest();

void *runLocalRedThread(void* data);

int runLocalReduction();

void reportError(int node);

#endif /* HEADERS_LOCALREDUCTION_H_ */
