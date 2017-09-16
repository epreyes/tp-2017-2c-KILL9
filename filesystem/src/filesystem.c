/*
 ============================================================================
 Name        : filesystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <connections/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include "header/fileSystem.h"
int main(void) {

	load_file_system_properties();
	//Mock fileSysten socket servidor
	// socket thread
	int * new_sock;
	listenning_socket = open_socket(file_system_config->file_system_puerto,	SOCKET_BACKLOG);
	for (;;) {
		new_sock = malloc(1);
		* new_sock = accept_connection(listenning_socket);
		//Procesamos las peticiones
		process_request(new_sock);
	}
	close_socket(listenning_socket);

	return EXIT_SUCCESS;
}




int process_request(int socket){
	PROTOCOL_DATANODE_TO_FILESYSTEM typeReceiveMessage;

	libtp_receiveMessage(socket, &tipoMensajeRecibido,
			sizeof(PROTOCOLO_KERNEL_A_FS), infoLogger);
	//TODO manejar peticiones
}


//TODO mover a otro archivo de funciones
void  load_data_node_properties_files(){
	config = config_create("./properties/filSystem.properties");
	file_system_config = malloc(sizeof(t_file_system_config));
	file_system_config ->file_system_puerto = config_get_string_value(config,"IP_FILESYSTEM");
	file_system_config ->file_system_puerto = config_get_string_value(config,"PORT_FILESYSTEM");
	free(config);
}
