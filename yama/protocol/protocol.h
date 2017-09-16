/*
 * protocol.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/*--------- Operations Codes -------*/
/*
 * TR: Transformation
 * LR: Local Reduction
 * GR: Global Reduction
 * FS: Final Store
 * */
typedef enum operation_code{
	T, L, G, S
}operation_code;

/*----------- HEADER ---------------*/
typedef struct{
	operation_code op;
	int msg_size;
	char file[20];
}header;

typedef struct{
	int msg_size;
	char file[20];
}fs_header;

/*------------------------- FileSystem Response -------------------*/
typedef struct t_copy{
	int node_id;
	int block_id;
}t_copy;

typedef struct block{
	int block_id;
	t_copy copy1;
	t_copy copy2;
	int end_block;
}block;

typedef struct file_info{
	int size;
	char type;
	int parent_dir;
	char status;
	block blocks[100];
}file_info;


/*------------------------- MASTER'S MESSAGES ---------------------*/
/*----------- RESPONSES ------------*/
typedef struct tr_datos{
	int		nodo;
	char	direccion[21];
	int		bloque;
	int		tamanio;
	char	tr_tmp[10];
}tr_datos;

typedef struct rl_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[10];
	char	rl_tmp[10];
}rl_datos;

typedef struct rg_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[10];
	char	rl_tmp[10];
	char	encargado;
}rg_datos;

typedef struct af_datos{
	int		nodo;
	char	direccion[21];
	char	encargado;
}af_datos;
/*----------------------- END MASTER'S MESSAGES ------------------*/


#endif /* PROTOCOL_H_ */
