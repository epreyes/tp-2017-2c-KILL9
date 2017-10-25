/*
 * connectionsManager.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"

void loadServer(void){

	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = INADDR_ANY;
	workerAddr.sin_port = htons(config_get_int_value(config,"WORKER_PUERTO"));
	int workerSocket= socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(workerSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(workerSocket, (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		log_error(logger,"No se pudo realizar el bind en puerto %d",config_get_int_value(config,"WORKER_PUERTO"));
		exit(1);
	}
	log_trace(logger, "Worker iniciado en puerto %d", config_get_int_value(config,"WORKER_PUERTO"));
	log_info(logger,"Esperando conexiones de master");
	listen(workerSocket, 100);

	struct sockaddr_in masterAddr;
	unsigned int len;

	int masterSocket = accept(workerSocket, (void*) &masterAddr, &len);
	if(masterSocket!=-1){
		log_trace(logger,"Master %d: Conectado",masterSocket);
	}else{
		log_error(logger,"Error al establecer conexión con Master");
		exit(1);
	}
	socket_worker=workerSocket;
	socket_master=masterSocket;
}

//==========================================================================

void readBuffer(){
	char operation;
	readSocketBuffer(socket_master,sizeof(char),&operation);
		//--------
		switch(operation){
			case 'T':
				log_trace(logger,"Master %d: Solicitud de transformación recibida",socket_master);
				transformation();
				break;
			case 'L':
				log_info(logger,"Master %d: Solicitud de Reducción Local recibida",socket_master);
				//localRedcution();
				break;
			case 'G':
				log_info(logger,"Master %d: Solicitud de Reducción Global recibida",socket_master);
				//globalReduction();
				break;
			case 'S':
				log_info(logger,"Master %d: Solicitud de Almacenado Final recibida",socket_master);
				//finalStorage();
				break;
			default:
				log_error(logger,"No existe la operación Solicitada");
				close(socket_master);
				break;
				//cerrar conexion y devolver error
		}
		//free(buffer);
}
