/*
 * globalReduction.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef HEADERS_GLOBALREDUCTION_H_
#define HEADERS_GLOBALREDUCTION_H_

#include "master.h"
#include "utils.h"

typedef struct dataNodes{
	int		node;
	char 	ip[16];
	int		port;
	char	rl_tmp[28];
}dataNodes;

typedef struct dataThread_GR{
	int				leadNode;
	char 			leadIp[16];
	int				leadPort;
	char			rl_tmp[28];
	char			rg_tmp[24];
	dataNodes* 		brothersData;
	int				brothersCount;
	threadMetrics	metrics;
}dataThread_GR;


int runGlobalReduction();

void reportGRError(int node);

#endif /* HEADERS_GLOBALREDUCTION_H_ */
