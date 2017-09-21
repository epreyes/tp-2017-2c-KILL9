/*
 * protocol.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/*----------- HEADER ---------------*/

typedef struct {
	char op;
	int msg_size;
} t_header;

typedef struct {
	t_header head;
	void* payload;
} t_package;

/*------------------------- FileSystem Response -------------------*/
typedef struct t_copy {
	int node_id;
	int block_id;
} t_copy;

typedef struct block {
	int block_id;
	int node;
	char node_ip[15];
	int node_port;
	int node_block;
	int end_block;
} block;

typedef struct file_info {
	int size;
	char type;
	int parent_dir;
	char status;
	block blocks[100];
} file_info;

/*------------------------- MASTER'S MESSAGES ---------------------*/
/*----------- RESPONSES ------------*/
typedef struct tr_datos {
	int nodo;
	char ip[15];
	long port;
	int bloque;
	int tamanio;
	char tr_tmp[28];
} tr_datos;

typedef struct rl_datos {
	int nodo;
	char ip[15];
	long port;
	char tr_tmp[10];
	char rl_tmp[10];
} rl_datos;

typedef struct rg_datos {
	int nodo;
	char direccion[21];
	char tr_tmp[10];
	char rl_tmp[10];
	char encargado;
} rg_datos;

typedef struct af_datos {
	int nodo;
	char direccion[21];
	char encargado;
} af_datos;
/*----------------------- END MASTER'S MESSAGES ------------------*/

#endif /* PROTOCOL_H_ */
