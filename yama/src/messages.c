/*
 * messages.c
 *
 *  Created on: 10/11/2017
 *      Author: utnso
 */

#include "headers/yama.h"

char* sendResponseMsg(int master, int bytes, void* response) {
	char op;
	memcpy(&op, response, sizeof(char));

	if( op == 'R'){
		op == 'T';
	}

	t_job* job = list_get(yama->tabla_jobs, getJobIndex(master, op));

	char* msg;
	switch (op) {
	case 'T':
		asprintf(&msg, "Respuesta de Transformacion: enviados %d bytes. Job %d.",
				bytes,job->id);
		break;
	case 'L':
		asprintf(&msg, "Respuesta de reduccion Local: enviados %d bytes. Job %d.",
				bytes,job->id);
		break;
	case 'G':
		asprintf(&msg, "Respuesta de reduccion Global: enviados %d bytes. Job %d.", bytes,
				job->id);
		break;
	case 'S':
		asprintf(&msg, "Respuesta de Almacenamiento Final: enviados %d bytes. Job %d.", bytes,
				job->id);
		break;
	case 'E':
			asprintf(&msg, "Error: enviados %d bytes. Job %d.", bytes,
					job->id);
			break;
	case 'R':
				asprintf(&msg, "Respuesta de Replanificacion: enviados %d bytes. Job %d.", bytes,
						job->id);
				break;
	}

	return msg;
}

char* printPlannigLogInfo(char* var, int value) {
	char* msg;
	asprintf(&msg, "%s = %d.", var, value);
	return msg;
}

void logNodeTable() {
	int index = 0;
	log_info(yama->log, "---------------- Carga de Nodos -----------------");
		for (index = 0; index < list_size(yama->tabla_nodos); index++) {
			elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
			log_info(yama->log, "Id: %d - Dispo: %d - En progreso: %d - Finalizadas: %d",
					node->node_id, node->availability, node->tasks_in_progress,
					node->tasts_done);
		}
		log_info(yama->log, "-------------------------------------------------\n");
}
