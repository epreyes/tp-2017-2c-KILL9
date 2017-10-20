/*
 * succesOperation.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/successOperation.h"

void processOk(int master) {

	void* buffer = malloc(sizeof(char));
	recv(master, buffer, sizeof(char), 0);
	char opCode;
	memcpy(&opCode, buffer, sizeof(char));
	free(buffer);

	buffer = malloc(sizeof(int));
	recv(master, buffer, sizeof(int), 0);
	int bloque;
	memcpy(&bloque, buffer, sizeof(int));
	free(buffer);

	buffer = malloc(sizeof(int));
	recv(master, buffer, sizeof(int), 0);
	int nodo;
	memcpy(&nodo, buffer, sizeof(int));
	free(buffer);

	printf("\nActualiza master %d, nodo %d, opCode %c, bloque %d \n", master, nodo, opCode, bloque);

	updateStatusTable(master, opCode, nodo, bloque, 'F');
	decreaseNodeCharge(nodo);

}
