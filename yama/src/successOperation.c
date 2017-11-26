/*
 * succesOperation.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/successOperation.h"
#include "headers/tablesManager.h"

char* getOpName(char op){
	switch(op){
	case 'T': return "Transformacion"; break;
	case 'L': return "Reduccion Local"; break;
	case 'G': return "Reduccion Global"; break;
	case 'S': return "Almacenamiento Final"; break;
	}
}

void* processOk(int master) {

	char opCode;
	recv(master, &opCode, sizeof(char), 0);

	int bloque;
	recv(master, &bloque, sizeof(int), 0);

	int nodo;
	recv(master, &nodo, sizeof(int), 0);

	int jobIndex = getJobIndex(master, opCode, 'P');
	t_job* job = list_get(yama->tabla_jobs, jobIndex);

	updateStatusTable(master, opCode, nodo, bloque, 'F', job);
	decreaseNodeCharge(nodo);

	log_info(yama->log, "Etapa de %s Terminada con exito, en nodo %d, bloque %d, Master %d.", getOpName(opCode), nodo, bloque, master);

	if( opCode == 'S' ){
		job->estado = 'F';
		list_replace(yama->tabla_jobs, jobIndex, job);
		log_trace(yama->log, "Job %d Finalizado con exito. Master %d.", job->id, master);
		deleteNodeErrors();
	}

	return "O";
}
