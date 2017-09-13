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
	char 	conector[21];
	char	rl_tmp[28];
}dataNodes;

typedef struct dataThread_GR{
	int			leadNode;
	char 		leadConector[21];
	char		rg_tmp[24];
	dataNodes* 	brothersData;
	int			brothersCount;
}dataThread_GR;


int runGlobalReduction(rg_datos yamaAnswer[], int totalRecords, metrics *masterMetrics);


#endif /* HEADERS_GLOBALREDUCTION_H_ */
