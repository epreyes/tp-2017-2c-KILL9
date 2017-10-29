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
	char	tmp[27];
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
	int		block;
	int		runtime;	//ver de pasar a real
	char	result;		//'E': error / 'O':"ok"
}tr_node_rs;

/*========LOCRED==========*/
//----Request----//
typedef char tmp[28];

typedef struct{
	char		code;				//red loc 'L'
	size_t		fileSize;			//tamaño del script
	char*		file;				//script
	tmp			rl_tmp;				//nombre del archivo de salida
	size_t		tmpsQuantity;		//cantidad de temporales a reducir
	tmp*		tr_tmp;				//nombre de temporales a reducir
}rl_node;

//-----Response---//
typedef struct{
	char	result;		//'E': error / 'O':"ok"
	int		runTime;
}rl_node_rs;


/*========GLOBAL_REDUCTION==========*/
typedef struct{
	int		node;
	char	ip[16];
	int		port;
	char	rl_tmp[28];
}rg_node;

//----Request----//
typedef struct{
	char		code;			//'G'
	char		rl_tmp[28];		//tmp del nodo maestro
	char		rg_tmp[24];		//nombre del archivo de salida
	int			nodesQuantity;
	rg_node*	nodes;			//datos de los nodos hermanos
}rg_node_rq;

//-----Response---//
typedef struct{
	int		runTime;
	char	result;		//'E': error / 'O':"ok"
}rg_node_rs;

/*========FINAL_STORAGE==========*/

typedef struct{
	char	code;		//'S'
	char	rg_tmp[24];
	int		fileNameSize;
	char*	fileName;			//Nombre del archivo Final
}fs_node_rq;

typedef struct{
	int		runTime;
	char	result;
}fs_node_rs;


#endif /* MASTER_WORKER_H_ */
