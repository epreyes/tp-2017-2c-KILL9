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

typedef struct block{
	int pos;
	int size;
}block;

typedef struct dataThread_TR{
	int		node;
	char 	conector[21];
	block* 	blocks;
	int		blocksCount;
}dataThread_TR;

void *runTransformThread(void* data);
int transformFile(tr_datos yamaAnswer[], int totalRecords, metrics *masterMetrics);

#endif /* HEADERS_TRANSFORM_H_ */
