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

	printf("\nActualizar estado del Nodo %d, bloque %d, operacion %c\n", nodo, bloque, opCode);
	updateStatusTable(master, opCode, nodo, bloque, 'F');

}
