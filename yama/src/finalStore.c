/*
 * finalStore.c
 *
 *  Created on: 19/10/2017
 *      Author: utnso
 */
#include "headers/finalStore.h"

int allGlobalReductionProcesFinish(int master, int jobid) {
	int response = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if (elem->job == jobid && elem->master == master && elem->op == 'G') {
			if (elem->status == 'P' || elem->status == 'E') {
				response = 0;
				return response;
			} else {
				response = 1;
			}
		}
	}
	return response;
}

void deleteOfGlobalReductionPlanedList(int items, int master) {
	int i = 0;
	for (i = 0; i < items; i++) {
		int j = 0;
		for (j = 0; j < list_size(yama->tabla_GR_planificados); j++) {
			elem_tabla_GR_planificados* elem = list_get(
					yama->tabla_GR_planificados, j);
			if (elem->master == master) {
				list_remove(yama->tabla_GR_planificados, j);
				break;
			}
		}

	}
}

t_list* findGlobalReductionPlaned(int master) {
	t_list* planed_list = list_create();
	int itemsToRemove = 0;

	int i = 0;
	for (i = 0; i < list_size(yama->tabla_GR_planificados); i++) {
		elem_tabla_GR_planificados* elem = list_get(yama->tabla_GR_planificados,
				i);
		if (elem->master == master) {
			list_add(planed_list, elem);
			itemsToRemove++;
		}
	}

	deleteOfGlobalReductionPlanedList(itemsToRemove, master);

	return planed_list;
}

void viewFinalStoreResponse(void* response) {
	char op;
	memcpy(&op, response, sizeof(char));

	int nodo;
	memcpy(&nodo, response + sizeof(char), sizeof(int));

	char* ip = malloc(sizeof(char) * 16);
	memcpy(ip, response + sizeof(char) + sizeof(int), sizeof(char) * 16);

	int puerto;
	memcpy(&puerto, response + sizeof(char) + sizeof(int) + sizeof(char) * 16,
			sizeof(int));

	char* fileName = malloc(sizeof(char) * 24);
	memcpy(fileName,
			response + sizeof(char) + sizeof(int) + sizeof(char) * 16
					+ sizeof(int), sizeof(char) * 24);

	printf(
			"\nFINAL STORE RESPONSE: Code=%c, Nodo=%d, ip=%s, puerto=%d, filename=%s\n",
			op, nodo, ip, puerto, fileName);
}

void destroyPlanedList(void* elem){
	free(((elem_tabla_GR_planificados*)elem)->data);
	free(elem);
}

void* processFinalStore(int master, int jobid) {
	if (allGlobalReductionProcesFinish(master, jobid)) {
		t_list* planed = findGlobalReductionPlaned(master);

		elem_tabla_GR_planificados* data = list_get(planed, 0);

		rg_datos* elem = malloc(sizeof(rg_datos));
		memcpy(elem, data->data, sizeof(rg_datos));

		int fsSize = sizeof(char) * 41 + sizeof(int) * 2;

		void* finalStoreRes = malloc(fsSize);

		memcpy(finalStoreRes, "S", sizeof(char));

		memcpy(finalStoreRes + sizeof(char), &(elem->nodo), sizeof(int));

		memcpy(finalStoreRes + sizeof(char) + sizeof(int), elem->ip,
				sizeof(char) * 16);

		memcpy(finalStoreRes + sizeof(char) + sizeof(int) + sizeof(char) * 16,
				&(elem->port), sizeof(int));

		memcpy(
				finalStoreRes + sizeof(char) + sizeof(int) + sizeof(char) * 16
						+ sizeof(int), elem->rg_tmp, sizeof(char) * 24);

		setInStatusTable(jobid, 'S', master, elem->nodo, 0,
				elem->rg_tmp, 0, data->fileName);
		increaseNodeCharge(elem->nodo);

		free(elem);

		list_destroy_and_destroy_elements(planed, &destroyPlanedList);

		return finalStoreRes;
	} else {
		log_warning(yama->log, "La reducción global debe terminar antes de empezar el almacenamiento final.");
		return NULL;
	}
}
