/*
 * errorsManager.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "headers/errorsManager.h"

void* abortJob(int master, int node, char op) {
	int size = sizeof(char)*2 + sizeof(int);
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

void* processError(int master) {
	void* response;

	void* op = malloc(sizeof(int));
	recv(master, op, sizeof(char), 0);

	void* node = malloc(sizeof(int));
	recv(master, node, sizeof(int), 0);

	switch (*(char*) op) {
	case 'T':
		response = replanTask(master, *(int*) node);
		break;
	case 'L':
	case 'G':
		response = abortJob(master, *(int*) node, *(char*)op);
		break;
	}
	return response;
}
