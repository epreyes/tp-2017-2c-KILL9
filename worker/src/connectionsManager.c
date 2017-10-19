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

	int workerSocket = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(workerSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(workerSocket, (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		perror("Falló el bind");
		exit(1);
	}

	printf("Estoy escuchando\n");
	listen(workerSocket, 100);

	struct sockaddr_in masterAddr;
	unsigned int len;
	int masterSocket = accept(workerSocket, (void*) &masterAddr, &len);
	printf("Recibí una conexión en %d!\n", masterSocket);


	//------------------------

	void* buffer = malloc(1);
	char charo;

	int bytesRecibidos = recv(masterSocket, buffer, 1, 0);
	if (bytesRecibidos <= 0) {
		perror("El chabón se desconectó o bla.");
		exit(1);
	}

	memcpy(&charo,buffer,1);
	printf("Código recibido:%c\n", charo);
	free(buffer);
}
