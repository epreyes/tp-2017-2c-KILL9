/*
 * errorsManager.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "headers/errorsManager.h"

void* abortJob(int master, int node, char op) {
	int size = sizeof(char) * 2 + sizeof(int);
	void* abort = malloc(size);
	char code = 'A';
	char opCode = op;
	int node_id = node;
	memcpy(abort, &code, sizeof(char));
	memcpy(abort + sizeof(char), &opCode, sizeof(char));
	memcpy(abort + sizeof(char) + sizeof(char), &node_id, sizeof(int));

	updateTasksAborted(master, node, op);

	return abort;
}

void* processNodeError(int master) {
	void* response;

	void* op = malloc(sizeof(int));
	recv(master, op, sizeof(char), 0);

	void* node = malloc(sizeof(int));
	recv(master, node, sizeof(int), 0);

	switch (*(char*) op) {
	case 'T':
		log_error(yama->log, "Error en Nodo %d. Comienza replanificacion. Job %d.", node, yama->jobs+master);
		response = replanTask(master, *(int*) node);
		log_info(yama->log, "Enviando informacion de replanificacion.");
		break;
	case 'L':
	case 'G':
		response = abortJob(master, *(int*) node, *(char*) op);
		log_error(yama->log, "Se aborta job %d. Master %d.", yama->jobs+master, master);
		break;
	}
	return response;
}

void showErrorMessage(void* response){
	char code;
	memcpy(&code, response, sizeof(char));

	int sizeMsg;
	memcpy(&sizeMsg, response+sizeof(char), sizeof(int));

	char* msg = malloc(sizeMsg);
	memcpy(msg, response+sizeof(char)+sizeof(int), sizeMsg);

	printf("\n%s\n", msg);
}

void* processErrorMessage(int master, char* errorMessage){
	int msgSize = strlen(errorMessage);
	void* response = malloc(sizeof(char)+sizeof(int)+msgSize);

	memcpy(response, "X", sizeof(char));
	memcpy(response+sizeof(char), &msgSize, sizeof(int));
	memcpy(response+sizeof(char)+sizeof(int), errorMessage, msgSize);

	return response;
}

void* invalidRequest(int master, char* errorMessage) {
	return processErrorMessage(master, errorMessage);
}
