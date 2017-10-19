/*
 * master_worker.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef MASTER_WORKER_H_
#define MASTER_WORKER_H_

#include <stdlib.h>

/*========TRANSFORM==========*/
//----Request----//
typedef struct{
	int 	pos;
	int 	size;
	char	tmp[28];
}block;

typedef struct{
	char		code;
	size_t		fileSize;
	char*		file;
	size_t		blocksSize;
	block*		blocks;
}tr_node;

//-----Response---//
typedef struct{
	char 	opcode;
	int		block;
	int		runtime;	//ver de pasar a real
	char	result;		//'E': error / 'O':"ok"
}tr_node_rs;

/*========LOCRED==========*/
//----Request----//
/*
typedef struct{
	int pos;
	int size;
}tr_tmp;

typedef struct{
	char		code;
	size_t		fileSize;
	char*		file;
	size_t		tr_tmpsSize;
	tr_tmp*		tr_tmps;
}tr_node;

//-----Response---//
typedef struct{
	char 	code;
	int		block;
	char	result;		//'E': error / 'O':"ok"
}tr_node_rs;

*/

#endif /* MASTER_WORKER_H_ */
