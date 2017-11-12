/*
 * connectionsManager.c
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"

void initializeNodeSockets(void){
	int i;
	for(i=0;i<100;++i){
		nodeSockets[i] = -1;
	};
}

void openYamaConnection(void) {
	struct sockaddr_in yamaAddr;
	char* yama_ip = config_get_string_value(config,"YAMA_IP");
	int yama_port = config_get_int_value(config,"YAMA_PUERTO");


	yamaAddr.sin_family = AF_INET;
	yamaAddr.sin_addr.s_addr = inet_addr(yama_ip);
	yamaAddr.sin_port = htons(yama_port);


	masterSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(masterSocket, (void*) &yamaAddr, sizeof(yamaAddr)) != 0) {
		log_error(logger,"No se pudo conectar con Yama (%s:%d) socket:%d",yama_ip,yama_port, masterSocket);
		exit(1);
	};

	log_info(logger,"conexión con Yama establecida (%s:%d) socket:%d",yama_ip,yama_port, masterSocket);
}

int openNodeConnection(int node, char* ip, int port){
/*
	int port;
	char* ip;
	ip=strtok(direction,":");
	port=atoi(strtok(NULL,":"));
*/
	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = inet_addr(ip);
	workerAddr.sin_port = htons(port);

	nodeSockets[node] = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(nodeSockets[node], (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		log_error(logger, "No se puede conectar con el nodo:%d (%s:%d)", node, ip, port);
		return 1;
	}else{
		log_info(logger, "Conexión con nodo %d establecida (puerto:%d-socket:%d)", node, port, nodeSockets[node]);
		return 0;
	}
}

int closeConnections(){
	int i=0;
	log_info(logger,"Cerrando Conexiones");
	if(close(masterSocket)<0){
		log_error(logger, "No se pudo cerrar la conexión con YAMA");
		return EXIT_FAILURE;
	}
	log_info(logger, "Conexión con YAMA cerrada");
	for(i=0;i<100;++i){
		if(nodeSockets[i]>0){
			if(close(nodeSockets[i])<0){
				log_error(logger, "Nodo %d: No es posible cerrar la conexión", i);
				return EXIT_FAILURE;
			}
			log_info(logger,"Nodo %d: Conexión cerrada", i);
		}
	}
	return EXIT_SUCCESS;
};
