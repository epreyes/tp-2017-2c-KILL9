/*
 * transform.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef HEADERS_TRANSFORM_H_
#define HEADERS_TRANSFORM_H_

#include "master.h"
#include "utils.h"

typedef struct dataThread_TR{
	int				node;
	char 			ip[16];
	int				port;
	block* 			blocks;
	int				blocksCount;
	threadMetrics 	metrics;
}dataThread_TR;

void *runTransformThread(void* data);

int transformFile(char* filename);

void sendRequest(char* fileName);

#endif /* HEADERS_TRANSFORM_H_ */
