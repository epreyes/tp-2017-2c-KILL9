/*
 * connectionsManager.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"
#include  <sys/types.h>
#include <tplibraries/protocol/master_worker.h>

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

	/*
	int masterSocket = accept(workerSocket, (void*) &masterAddr, &len);
	if(masterSocket!=-1){
		log_trace(logger,"Master %d: Conectado",masterSocket);
	}else{
		log_error(logger,"Error al establecer conexión con Master");
		exit(1);
	}
	socket_master=masterSocket;
	
	*/
	socket_worker=workerSocket;
	
}

//=======================FILESYSTEM=============================================

void openFileSystemConnection(void) {
	struct sockaddr_in fileSystemAddr;
	char* fileSystem_ip = config_get_string_value(config,"IP_FILESYSTEM");
	int fileSystem_port = config_get_int_value(config,"PUERTO_FILESYSTEM");

	fileSystemAddr.sin_family = AF_INET;
	fileSystemAddr.sin_addr.s_addr = inet_addr(fileSystem_ip);
	fileSystemAddr.sin_port = htons(fileSystem_port);

	socket_filesystem = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(socket_filesystem, (void*) &fileSystemAddr, sizeof(fileSystemAddr)) != 0) {
		log_warning(logger,"No se pudo conectar con Filesystem (%s:%d)",fileSystem_ip,fileSystem_port);
		error(1);	//ver como manejar
	};
	log_info(logger,"conexión con Filesystem establecida (%s:%d)",fileSystem_ip,fileSystem_port);
}


//=======================NODE=============================================
int openNodeConnection(int node, char* ip, int port){

	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = inet_addr(ip);
	workerAddr.sin_port = htons(port);

	socket_nodes[node] = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(socket_nodes[node], (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		log_error(logger, "No se puede conectar con el nodo:%d (%s:%d)", node, ip, port);
		return 1;
	}else{
		log_info(logger, "Conexión con nodo %d establecida (puerto:%d-socket:%d)", node, port, socket_nodes[node]);
		return 0;
	}
}

//==========================================================================

void startWorkerServer(){
	struct sockaddr_in masterAddr;
	unsigned int len;
	char operation;
	pid_t pid ;
	int handshak = 0; //Default
	int socket_client ;
	while(1){
	
		socket_client = accept(socket_worker, (void*) &masterAddr, &len);
		if(socket_client == -1){
			log_error(logger,"Error al establecer conexión con el cliente");
			continue;
		}
		log_info(logger,"Cliente conectado %d: Conectado desde %s\n",socket_client,inet_ntoa(masterAddr.sin_addr));

		//Espero codigo de operacion del cliente
		readBuffer(socket_client,sizeof(char),&operation);
				//--------
		switch (operation) {
		case 'T':
			log_trace(logger, "Master %d: Solicitud de transformación recibida",
					socket_client);
			transformation(socket_client);
			break;
		case 'L':
			log_info(logger, "Master %d: Solicitud de Reducción Local recibida",
					socket_client);
			//localReduction(socket_client);
			break;
		case 'G':
			log_info(logger,
					"Master %d: Solicitud de Reducción Global recibida",
					socket_client);
			globalReduction(socket_client);
			break;
		case 'S':
			log_info(logger,
					"Master %d: Solicitud de Almacenado Final recibida",
					socket_client);
			finalStorage(socket_client);
			break;

		case 'R':
			log_info(logger,"Worker %d: Solicitud de reduccion ",socket_client);
			//TODO Leer el nombre del archivo y responder al worker
			sendNodeFile(socket_client);
			break;
		default:
			log_error(logger, "No existe la operación Solicitada %s",operation);
			break;

		log_info(logger, "Fin de procesamiento solicitud de master ... \n");
	   }
	}
}

