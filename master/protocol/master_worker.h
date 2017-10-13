/*
 * master_worker.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef MASTER_WORKER_H_
#define MASTER_WORKER_H_

/*========TRANSFORM==========*/
//----Request----//
typedef struct block{
	int pos;
	int size;
}block;

typedef struct{
	char		code;
	size_t		fileSize;
	char*		file;
	size_t		blocksSize;
	block*		blocks;
}tr_node;

#endif /* MASTER_WORKER_H_ */