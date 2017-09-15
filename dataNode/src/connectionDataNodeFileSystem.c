/*
 * connectionDataNodeFileSystem.c
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */
#include "headers/connectionDataNodeFileSystem.h"

void connectionToFileSystem(){

	int result = connect_to_servidor(data_node_conf->file_system_ip,
			data_node_conf->file_system_port,&fileSystemSocket);

	if(result != 0){
		log_info(infoLogger,"Error al conectar al fileSystem.");
		perror("No se puede conectar al fileSystem");
		exit(EXIT_FAILURE);
	}

	PROTOCOL_DATANODE_TO_FILESYSTEM handshake = HANDSHAKE_CONNECT_DATANODE_TO_FILESYSTEM;
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
			//TODO tirar magia -> atender get y set de bloques de datos.

		}
		break;
	default:
		log_info(infoLogger,"-- Error al recibir los datos del FILESYSTEM");
		printf("-- Error al recibir los datos del FILESYSTEM ");
		break;

	}
}
