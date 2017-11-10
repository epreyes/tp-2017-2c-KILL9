/*
 * messages.c
 *
 *  Created on: 10/11/2017
 *      Author: utnso
 */

#include "headers/yama.h"

char* masterConnectedMsg(int master) {
	char* msg;
	asprintf(&msg, "Master %d conectado!", master);
	return msg;
}

char* masterDisconnectedMsg(int master) {
	char* msg;
	asprintf(&msg, "Master %d desconectado!", master);
	return msg;
}

char* processOperationMsg(int master, int op) {
	char* msg;
	switch (op) {
	case 'T':
		asprintf(&msg, "Solicitud de transformación. Job %d.",
				yama->jobs + master);
		break;
	case 'L':
		asprintf(&msg, "Solicitud de Reducción Local. Job %d.",
				yama->jobs + master);
		break;
	case 'G':
		asprintf(&msg, "Solicitud de Reducción Global. Job %d",
				yama->jobs + master);
		break;
	case 'S':
		asprintf(&msg, "Solicitud de Almacenado Final. Job %d.",
				yama->jobs + master);
		break;
	}

	return msg;
}

char* startTransformationMsg(int master) {
	char* msg;
	asprintf(&msg, "Atendiendo solicitud de Transformación. Job %d.",
			yama->jobs + master);
	return msg;
}

char* gettingFileInfoMsg(char* fileName, int master) {
	char* msg;
	asprintf(&msg, "Solicitando informacion del archivo: %s. Job %d.", fileName,
			yama->jobs + master);
	return msg;
}

char* gettingFileInfoFromMsg(char* from, int master) {
	char* msg;
	asprintf(&msg, "Obteniendo informacion de %s. Job %d.", from,
			yama->jobs + master);
	return msg;
}

char* sendResponseMsg(int master, int bytes, void* response) {
	char op;
	memcpy(&op, response, sizeof(char));

	char* msg;
	switch (op) {
	case 'T':
		asprintf(&msg, "Transformation Response: sent %d bytes to Job %d.",
				bytes, yama->jobs + master);
		break;
	case 'L':
		asprintf(&msg, "Local Reduction Response: sent %d bytes to Job %d.",
				bytes, yama->jobs + master);
		break;
	case 'G':
		asprintf(&msg, "Global Reduction: sent %d bytes to Job %d.", bytes,
				yama->jobs + master);
		break;
	case 'S':
		asprintf(&msg, "Final Store Response: sent %d bytes to Job %d.", bytes,
				yama->jobs + master);
		break;
	}

	return msg;
}

char* planningErrorMsg(int op, int node, int master) {
	char* msg;
	asprintf(&msg, "Error en el nodo %d, operación %c, Job %d.", node, op,
			yama->jobs + master);
	return msg;
 	 }

char* printPlannigLogInfo(char* var, int value) {
	char* msg;
	asprintf(&msg, "%s = %d.", var, value);
	return msg;
}

char* printNodeTable() {
	char* msg;
	int index = 0;
	asprintf(&msg, "| nodo id | avail | in progres | done |\n");
		for (index = 0; index < list_size(yama->tabla_nodos); index++) {
			elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
			asprintf(&msg, "| %d | %d | %d | %d |\n",
					node->node_id, node->availability, node->tasks_in_progress,
					node->tasts_done);
		}

	return msg;
}
