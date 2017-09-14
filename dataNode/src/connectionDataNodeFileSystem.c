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

	PROTOCOLO_DATANODE_TO_FILESYSTEM handshake = HANDSHAKE_CONNECT_DATANODE_TO_FILESYSTEM;

	libtp_sendMessage(fileSystemSocket, &handshake,
				sizeof(PROTOCOLO_DATANODE_TO_FILESYSTEM), infoLogger);

	//TODO manejar de acuerdo al resultado enviado por filesystem
	switch(result){
	case  :
		break;

	default:
		break;

	}
}
