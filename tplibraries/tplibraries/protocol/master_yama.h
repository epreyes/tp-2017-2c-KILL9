/*
 * master_yama.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef MASTER_YAMA_H_
#define MASTER_YAMA_H_

#include <stdlib.h>

/*========TRANSFORM==========*/
//----Request----//
typedef struct{
	char	code;
	size_t	fileNameSize;
	char*	fileName;
}transform_rq;

//----Response----//
typedef struct{
	int		nodo;
	char	ip[16];
	int 	port;
	int		bloque;
	int		tamanio;
	char	tr_tmp[28];
}tr_datos;

typedef struct{
	char		code;
	int			bocksQuantity;
	tr_datos*	blockData;
}transform_rs;

/*========LOCAL_RED==========*/

//----Request----//

typedef struct{
	char	code;
}local_rq;

//----Response----//

typedef struct{
	int		nodo;
	char	ip[16];
	int 	port;
	char	tr_tmp[28];
	char	rl_tmp[28];
}rl_datos;

typedef struct{
	char		code;
	int			blocksQuantity;
	rl_datos*	blockData;
}local_rs;

/*=======GLOBAL_RED==========*/
//----Request----//
typedef struct{
	char	code;
}global_rq;

//----Response----//
typedef struct rg_datos{				//es un record por nodo
	int		nodo;
	char	ip[16];
	int 	port;
	char	rl_tmp[28];
	char	rg_tmp[28];
	char	encargado;
}rg_datos;

typedef struct{
	char		code;
	int			bocksQuantity;
	rl_datos*	blocksData;
}global_rs;

/*=======FINAL_STORAGE==========*/

//----Request----//
typedef struct{
	char	code;
}store_rq;

//----Response----//

typedef struct{
	char	code;
	int		nodo;
	char	ip[16];
	int 	port;
	char	rg_tmp[24];
}store_rs;

typedef struct {
	int		nodo;
	char	ip[16];
	int 	port;
	char	rg_tmp[24];
}af_datos;

/*=======ERRORS==========*/
typedef struct{
	char	code;
	char	opCode;
	int		nodo;
}error_rq;

/*======= OK ==========*/
typedef struct{
	char	code;		//'O'
	char	opCode;		//'T','L','G','S'
	int 	bloque;		// 0 si es opCode != 'T'
	int		nodo;
}ok;

#endif /* MASTER_YAMA_H_ */
