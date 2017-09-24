/*
 * data_node_prot.h
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */

#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef CONNECTIONS_DATA_NODE_PROT_H_
#define CONNECTIONS_DATA_NODE_PROT_H_



#define GET_BLOQUE       	  2
#define	DISCONNECTED_CLIENT  -201
#define	DISCONNECTED_SERVER	 -202

#define	SUCCESS				   1

int recv_operation_code(int * client_socket, t_log * logger);

/**
 * DN - GET BLOQUE
 */

typedef struct {
	int16_t exec_code;
	uint32_t buffer_size;
	void * buffer;
}t_dn_get_block_resp;

typedef struct{
	int16_t exec_code;
	uint32_t number_block;
}t_dn_get_block_req;



t_dn_get_block_resp dn_get_block(int server_socket,int number_block,t_log * logger);

t_dn_get_block_req * dn_get_block_recv_req(int * client_socket, t_log * logger) ;

void dn_get_block_resp(int * client_socket, int resp_code, int buffer_size, void * buffer);




#endif /* CONNECTIONS_DATA_NODE_PROT_H_ */
