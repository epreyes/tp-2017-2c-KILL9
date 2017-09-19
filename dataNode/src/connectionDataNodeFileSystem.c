/*
 * connectionDataNodeFileSystem.c
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */
#include "headers/connectionDataNodeFileSystem.h"
#include "headers/dataNode.h"


void connectionToFileSystem(){

	fileSystemSocket = connect_to_socket(data_node_config-> file_system_ip,data_node_config-> file_system_port);
	if(result != 0){
		log_info(infoLogger,"Error al conectar al fileSystem.");
		perror("No se puede conectar al fileSystem");
		exit(EXIT_FAILURE);
	}

	PROTOCOL_DATANODE_TO_FILESYSTEM handshake = HANDSHAKE_CONNECTION_DATANODE_TO_FILESYSTEM;
	//Envio  mensaje al file system
	libtp_sendMessage(fileSystemSocket, &handshake,sizeof(PROTOCOL_DATANODE_TO_FILESYSTEM), infoLogger);

	//Espero respuesta del filesytem
	PROTOCOL_FILESYSTEM_TO_DATANODE handshakeResponse;
	result =  libtp_receiveMessage(fileSystemSocket,&handshakeResponse,
			sizeof(PROTOCOL_FILESYSTEM_TO_DATANODE),infoLogger);

	//Manejo respuesta del FILESYTEM
	switch(result){
	case CLOSE_CONNECTION :
		log_info(infoLogger,"-> Se  desconecto el FILESYSTEM");
		printf("Se desconecto el FILESYSTEM");
		break;
	case RECEIVE_OK:
		if(handshakeResponse == HANDSHAKE_CONNECTION_FILESYSTEM_TO_DATANODE_OK){
				process_request_file_system(fileSystemSocket);
		}
		break;
	default:
		log_info(infoLogger,"-- Error al recibir los datos del FILESYSTEM");
		printf("-- Error al recibir los datos del FILESYSTEM ");
		break;

	}
}

void process_request_file_system(int * client_socket) {
	int ope_code = recv_operation_code(client_socket, logger);
	while (ope_code != DISCONNECTED_CLIENT) {
		log_info(logger, "CLIENT %d >> codigo de operacion : %d", * client_socket, ope_code);
		switch (ope_code) {
			case GET_BLOQUE:
					get_block(client_socket);
			  break;
			default:
				  set_block(client_socket);
				break;
		}
		ope_code = recv_operation_code(client_socket, logger);
	}
		close_client(* client_socket);
		free(client_socket);
		return;
}
/*****************************************************
*	Implementacion para leer un bloque de un archivo   *
******************************************************/
void get_block(int client_socket){
		t_dn_get_block_req * request = dn_get_block_recv_req(client_socket,logger);
		//TODO obtener el puntero al archivo memoria , leer y enviar las respues al cliente.	

}
