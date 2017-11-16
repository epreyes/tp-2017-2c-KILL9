/*
 * succesOperation.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/successOperation.h"
#include "headers/tablesManager.h"

void* processOk(int master) {

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

	int jobIndex = getJobIndex(master, opCode, 'P');
	t_job* job = list_get(yama->tabla_jobs, jobIndex);

	updateStatusTable(master, opCode, nodo, bloque, 'F', job);
	decreaseNodeCharge(nodo);

	log_info(yama->log, "Operacion %c OK en nodo %d, bloque %d, Master %d.", opCode, nodo, bloque, master);

	if( opCode == 'S' ){
		job->estado = 'F';
		list_replace(yama->tabla_jobs, jobIndex, job);
		log_trace(yama->log, "Job %d Finalizado con exito. Master %d.", job->id, master);
	}

	buffer = malloc(sizeof(char));
	memcpy(buffer, "O", sizeof(char));

	return buffer;
}
