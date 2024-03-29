/*
 * localReduction.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"

#include "headers/tablesManager.h"

void deleteOfPlanedList(int items, int master) {
	int i = 0;
	for (i = 0; i < items; i++) {
		int j = 0;
		for (j = 0; j < list_size(yama->tabla_T_planificados); j++) {
			elem_tabla_planificados* elem = list_get(yama->tabla_T_planificados,
					j);
			if (elem->master == master) {
				list_remove(yama->tabla_T_planificados, j);
				break;
			}
		}

	}
}

t_list* findTransformationPlaned(int master, int jobid) {
	t_list* planed_list = list_create();
	int itemsToRemove = 0;

	int i = 0;
	for (i = 0; i < list_size(yama->tabla_T_planificados); i++) {
		elem_tabla_planificados* elem = list_get(yama->tabla_T_planificados, i);
		if (elem->job == jobid && elem->master == master) {
			list_add(planed_list, elem);
			itemsToRemove++;
		}
	}

	deleteOfPlanedList(itemsToRemove, master);

	return planed_list;
}

void viewLocalReductionResponse(void* response) {
	char* op = malloc(sizeof(char));
	memcpy(op, response, sizeof(char));
	int* bloques = malloc(sizeof(int));
	memcpy(bloques, response + sizeof(char), sizeof(int));

	int plus = sizeof(int) + sizeof(char);
	int i = 0;
	for (i = 0; i < *bloques; i++) {
		rl_datos* data = malloc(sizeof(rl_datos));
		memcpy(data, response + plus + (i * sizeof(rl_datos)),
				sizeof(rl_datos));
	}
}

void getLocalReductionTmpName(char* name, int blockId, int masterId) {
	char* nameTmp;
	long timestamp = current_timestamp();
	asprintf(&nameTmp, "%s%ld-L-M%03d-N%03d", "/tmp/", timestamp, masterId, blockId);
	strcpy(name, nameTmp);

	free(nameTmp);
}

int allTransformProcesFinish(int master, int jobid) {
	int response = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if ( elem->job == jobid && elem->master == master && elem->op == 'T') {
			if (elem->status == 'P') {
				response = 0;
				return response;
			} else {
				response = 1;
			}
		}
	}
	return response;
}

void destroyTablePlanned(void* elem){
	free(((elem_tabla_planificados*)elem)->data);
	free(elem);
}

void* processLocalReduction(int master, int job) {
	if (allTransformProcesFinish(master, job)) {
		t_list* planed = findTransformationPlaned(master, job);

		void* localReductionRes = malloc(
				sizeof(char) + sizeof(int)
						+ sizeof(rl_datos) * list_size(planed));
		memcpy(localReductionRes, "L", sizeof(char));
		int size = list_size(planed);
		memcpy(localReductionRes + sizeof(char), &size, sizeof(int));

		int index = 0;
		int actualNode = -1;
		char* name = malloc(sizeof(char)*28);

		for (index = 0; index < list_size(planed); index++) {
			elem_tabla_planificados* elemData = list_get(planed, index);
			tr_datos* elem = malloc(sizeof(tr_datos));
			memcpy(elem, elemData->data, sizeof(tr_datos));
			rl_datos* localRedData = malloc(sizeof(rl_datos));
			localRedData->nodo = elem->nodo;
			localRedData->port = elem->port;
			strcpy(localRedData->ip, elem->ip);
			strcpy(localRedData->tr_tmp, elem->tr_tmp);

			if( actualNode != elem->nodo ){
				actualNode = elem->nodo;
				getLocalReductionTmpName(name, elem->nodo, master);
			}
			strcpy(localRedData->rl_tmp, name);

			free(elem);

			if (!existInStatusTable(job, 'L',
					localRedData->nodo)) {
				setInStatusTable(job, 'L', master, localRedData->nodo,
						getBlockId(localRedData->rl_tmp), localRedData->rl_tmp,
						0, elemData->fileName);

				//creo el elemento para agregar a la tabla de planificados.
				addToLocalReductionPlanedTable(master, localRedData, elemData->fileName, job);
				increaseNodeCharge(localRedData->nodo);
			}

			memcpy(
					localReductionRes + sizeof(char) + sizeof(int)
							+ (index * sizeof(rl_datos)), localRedData,
					sizeof(rl_datos));

			free(localRedData);
		}

		free(name);

		list_destroy_and_destroy_elements(planed, &destroyTablePlanned);
		return localReductionRes;
	} else {
		log_warning(yama->log, "Todas las transformaciones deben terminar antes de empezar las reducciones locales.");
		return "E";
	}

}
