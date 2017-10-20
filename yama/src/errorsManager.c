/*
 * errorsManager.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "headers/errorsManager.h"

void* processError(int master) {
	void* response;
	void* node = malloc(sizeof(int));
	recv(master, node, sizeof(int), 0);

	void* op = malloc(sizeof(int));
	recv(master, op, sizeof(char), 0);

	switch (*(char*) op) {
	case 'T':
		response = replanTask(master, node);
		break;
	case 'L':
	case 'G':
		response = abortJob(master, node);
		break;
	}
	return response;
}
