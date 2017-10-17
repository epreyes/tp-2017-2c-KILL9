/*
 * connectionsManager.c
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"

void openYamaConnection(void) {
	struct sockaddr_in yamaAddr;
	yamaAddr.sin_family = AF_INET;
	yamaAddr.sin_addr.s_addr = inet_addr(config_get_string_value(config,"YAMA_IP"));
	yamaAddr.sin_port = htons(config_get_int_value(config,"YAMA_PUERTO"));

	/*
	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(masterSocket, (void*) &yamaAddr, sizeof(yamaAddr)) != 0) {
		perror("No se pudo conectar");
	}
	*/
}

int openNodeConnection(int node, char* direction){

	int port;
	char* ip;
	ip=strtok(direction,":");
	port=atoi(strtok(NULL,":"));


	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = inet_addr(ip);
	workerAddr.sin_port = htons(port);

	nodeSockets[node] = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(nodeSockets[node], (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		printf("No se pudo conectar con el nodo:%d\n", node);
		return 1;
	}
	return 0;
}
