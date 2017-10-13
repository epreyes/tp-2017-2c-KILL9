/*
 * protocol.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/*========TRANSFORM==========*/
//----Request----//
typedef struct{
	char	code;
	size_t	fileNameSize;
	char*	fileName;
}transform_rq;

//----Response----//
typedef struct tr_datos{
	int		nodo;
	char	direccion[21];
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

typedef struct rl_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[28];
	char	rl_tmp[28];
}rl_datos;

typedef struct{
	char		code;
	int			bocksQuantity;
	rl_datos*	blocksData;
}local_rs;

/*=======GLOBAL_RED==========*/
//----Request----//
typedef struct{
	char	code;
}global_rq;

//----Response----//
typedef struct rg_datos{				//es un record por nodo
	int		nodo;
	char	direccion[21];
	char	rl_tmp[28];
	char	rg_tmp[24];
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
	char	direccion[21];
	char	rg_tmp[24];
}store_rs;

typedef struct af_datos{
	int		nodo;
	char	direccion[21];
	char	rg_tmp[24];
}af_datos;

/*=======ERRORS==========*/
typedef struct{
	char	code;
	char	type;
	int		nodo;
}error_rq;

/*
 * '0': "no se puede conectar con el nodo Y"
 * '1': "no es posible procesar el bloque X del nofo Y"
*/
#endif /* PROTOCOL_H_ */