/*
 * connectionsManager.c
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"


void openYamaConnection(void) {
	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = inet_addr(config_get_string_value(config,"YAMA_IP"));
	workerAddr.sin_port = htons(config_get_int_value(config,"YAMA_PUERTO"));

	masterSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(masterSocket, (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		perror("No se pudo conectar");
	}
}
