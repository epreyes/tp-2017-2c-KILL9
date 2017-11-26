/*
 * errorsManager.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "headers/errorsManager.h"

void* abortJob(int master, int node, char op, t_job* job) {
	int size = sizeof(char) * 2 + sizeof(int);
	void* abort = malloc(size);
	char code = 'A';
	char opCode = op;
	int node_id = node;
	memcpy(abort, &code, sizeof(char));
	memcpy(abort + sizeof(char), &opCode, sizeof(char));
	memcpy(abort + sizeof(char) + sizeof(char), &node_id, sizeof(int));

	updateTasksAborted(master, node, op, job);
	deleteNodeErrors();

	return abort;
}

void* processNodeError(int master) {
	void* response;

	char op;
	recv(master, &op, sizeof(char), 0);

	int node;
	recv(master, &node, sizeof(int), 0);

	int jobIndex = getJobIndex(master, op, 'P');
	t_job* job = list_get(yama->tabla_jobs, jobIndex);

	switch (op) {
	case 'T': {
		log_error(yama->log,
				"Error en Nodo %d. Comienza replanificacion. Job %d.",
				node, job->id);
		t_planningParams* params = getPlanningParams();
		strcpy(params->algoritm, yama->algoritm);
		params->availBase = yama->availBase;
		params->planningDelay = yama->planningDelay;
		deleteFromPlanedTable(master, node);

		increaseNodeError( node );
		response = replanTask(master, node, params, job, jobIndex);
		viewNodeTable();

		free(params);

		if( job->replanificaciones == 1 ){
			log_info(yama->log, "Enviando informacion de replanificacion.");
		}else{
			log_info(yama->log, "Enviando informacion de Job Abortado.");
		}

	}
		break;
	case 'L':
	case 'G':
	case 'S': {
		log_trace(yama->log, "Recivo error en operacion %c. Job %d.",
				op, job->id);
		response = abortJob(master, node, op, job);
		log_error(yama->log, "Se aborta job %d. Master %d.",
				job->id, master);
		job->estado = 'E';
		list_replace(yama->tabla_jobs, jobIndex, job);
	}
		break;
	}
	return response;
}

void showErrorMessage(void* response) {
	char code;
	memcpy(&code, response, sizeof(char));

	int sizeMsg;
	memcpy(&sizeMsg, response + sizeof(char), sizeof(int));

	char* msg = malloc(sizeMsg);
	memcpy(msg, response + sizeof(char) + sizeof(int), sizeMsg);

	log_error(yama->log, "%s", msg);
}

void* processErrorMessage(int master, char* errorMessage) {
	int msgSize = strlen(errorMessage);
	void* response = malloc(sizeof(char) + sizeof(int) + msgSize);

	memcpy(response, "X", sizeof(char));
	memcpy(response + sizeof(char), &msgSize, sizeof(int));
	memcpy(response + sizeof(char) + sizeof(int), errorMessage, msgSize);

	return response;
}

void* invalidRequest(int master, char* errorMessage) {
	return processErrorMessage(master, errorMessage);
}
