/*
 * data_node_prot.c
 *
 *  Created on: 18/9/2017
 *      Author: utnso
 */


#include <commons/log.h>
#include <stdint.h>
#include <stdlib.h>
#include "data_node_prot.h"
#include "socket.h"

/**
 * DATANODE - RECEIVE OPERATION CODE
 */
int recv_operation_code(int * client_socket, t_log * logger) {
	uint8_t prot_ope_code = 1;
	uint8_t ope_code;
	int received_bytes = socket_recv(client_socket, &ope_code, prot_ope_code);
	if (received_bytes <= 0) {
		if (logger) log_error(logger, "------ CLIENT %d >> disconnected", * client_socket);
		return DISCONNECTED_CLIENT;
	}
	return ope_code;
}

/***************************************
 * 		 DN - GET BLOQUE		   *							   *
 ***************************************/
t_dn_get_block_resp * dn_get_block(int server_socket,int number_block,t_log * logger){

	/** operation_code (1 byte),number_block (4 bytes)	**/
	uint8_t prot_ope_code = 1;
	uint8_t prot_number_block = 4;

	uint8_t  req_ope_code = GET_BLOQUE;
	uint32_t req_number_block = number_block;

	//tamanio del mensaje
	int msg_size = sizeof(char)*(prot_ope_code + prot_number_block);

	//Request
	void* request = malloc(msg_size);
	memcpy(request,&req_ope_code,prot_ope_code);
	memcpy(request + prot_ope_code,&req_number_block,prot_number_block);

	//Envio el mensaje
	socket_send(&server_socket, request, msg_size, 0);
	free(request);

	t_dn_get_block_resp * response = malloc(sizeof(t_dn_get_block_resp));

	//Obtengo el codigo de operacion
	uint8_t resp_prot_code = 2;
	int received_bytes = socket_recv(&server_socket, &(response->exec_code),
			resp_prot_code);
	if (received_bytes <= 0) {
		if (logger){
			log_error(logger, "SERVER %d -> disconnected",server_socket);
		}
		response->exec_code = DISCONNECTED_SERVER;
		return response;
	}
	//Obtengo el tamanio del buffer
	uint8_t resp_prot_buff_size = 4;
	received_bytes = socket_recv(&server_socket, &(response->buffer_size),resp_prot_buff_size);
	if (received_bytes <= 0) {
		if (logger){
			log_error(logger, "SERVER %d -> disconnected",server_socket);
		}
		response->exec_code = DISCONNECTED_SERVER;
		return response;
	}
	if ((response->buffer_size) > 0) {
		response->buffer = malloc((response->buffer_size));
		//Obtengo los datos del buffer
		received_bytes = socket_recv(&server_socket, (response->buffer),(response->buffer_size));
		if (received_bytes <= 0) {
			if (logger){
				log_error(logger, "SERVER %d -> disconnected",server_socket);
			}
			response->exec_code = DISCONNECTED_SERVER;
			return response;
		}
	}
	return response;
}

t_dn_get_block_req * dn_get_block_recv_req(int * client_socket, t_log * logger) {
	t_dn_get_block_req * request = malloc(sizeof(t_dn_get_block_req));
	uint8_t prot_req_number_block = 4;
	int received_bytes = socket_recv(client_socket, &(request->number_block), prot_req_number_block);
	if (received_bytes <= 0) {
		if (logger){
			log_error(logger, "CLIENT %d -> disconnected", * client_socket);
		}
		request->exec_code = DISCONNECTED_CLIENT;
		return request;
	}
	// Se recibieron todos los datos
	request->exec_code = SUCCESS;
	return request;
}

void dn_get_block_resp(int * client_socket, int resp_code, int buffer_size, void * buffer) {
	uint8_t resp_prot_code = 2;
	uint8_t resp_prot_buff_size = 4;
	int response_size = sizeof(char) * (resp_prot_code + resp_prot_buff_size + ((buffer_size > 0) ? buffer_size : 0));
	void * response = malloc(response_size);
	memcpy(response, &resp_code, resp_prot_code);
	memcpy(response + resp_prot_code, &buffer_size, resp_prot_buff_size);
	if (buffer_size > 0) {
		memcpy(response + resp_prot_code + resp_prot_buff_size, buffer, buffer_size);
	}
	socket_write(client_socket, response, response_size);
	free(response);
}


