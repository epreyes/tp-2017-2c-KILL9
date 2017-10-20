/*
 * localReduction.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"



void deleteOfPlanedList(int items, int master) {
	int i = 0;
	for(i = 0; i < items; i++){
		int j = 0;
		for(j = 0; j < list_size(yama->tabla_T_planificados); j++){
			elem_tabla_planificados* elem = list_get(yama->tabla_T_planificados, j);
			if( elem->master == master){
				list_remove(yama->tabla_T_planificados, j);
				break;
			}
		}

	}
}

t_list* findTransformationPlaned(int master) {
	t_list* planed_list = list_create();
	int itemsToRemove = 0;

	int i = 0;
	for (i = 0; i < list_size(yama->tabla_T_planificados); i++) {
		elem_tabla_planificados* elem = list_get(yama->tabla_T_planificados, i);
		if (elem->master == master) {
			list_add(planed_list, elem->data);
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
	printf("\nLa respuesta de la reduccion local es (%c, %d, %d): ", *op,
			*bloques, (*bloques) * sizeof(rl_datos));
	int plus = sizeof(int) + sizeof(char);
	int i = 0;
	for (i = 0; i < *bloques; i++) {
		rl_datos* data = malloc(sizeof(rl_datos));
		memcpy(data, response + plus + (i * sizeof(rl_datos)),
				sizeof(rl_datos));
		printf("\nNodo=%d - Ip=%s - Puerto=%d - TR_TMP=%s - RL_TMP=%s\n",
				data->nodo, data->ip, data->port, data->tr_tmp, data->rl_tmp);
	}
}

void getLocalReductionTmpName(rl_datos* nodeData, int op, int blockId,
		int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);

	strcpy(nodeData->rl_tmp, name);
}

int allTransformProcesFinish(int master) {
	viewStateTable();
	int response = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if (elem->master == master && elem->op == 'T') {
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

void* processLocalReduction(int master) {
	if (allTransformProcesFinish(master)) {
		t_list* planed = findTransformationPlaned(master);

		void* localReductionRes = malloc(
				sizeof(char) + sizeof(int)
						+ sizeof(rl_datos) * list_size(planed));
		memcpy(localReductionRes, "L", sizeof(char));
		int size = list_size(planed);
		memcpy(localReductionRes + sizeof(char), &size, sizeof(int));

		int index = 0;
		for (index = 0; index < list_size(planed); index++) {
			tr_datos* elem = list_get(planed, index);
			rl_datos* localRedData = malloc(sizeof(rl_datos));
			localRedData->nodo = elem->nodo;
			localRedData->port = elem->port;
			strcpy(localRedData->ip, elem->ip);
			strcpy(localRedData->tr_tmp, elem->tr_tmp);
			getLocalReductionTmpName(localRedData, 'L',
					getBlockId(elem->tr_tmp), master);
			setInStatusTable('L', master, localRedData->nodo,
					getBlockId(localRedData->rl_tmp), localRedData->rl_tmp);

			//creo el elemento para agregar a la tabla de planificados.
			addToLocalReductionPlanedTable(master, localRedData);
			increaseNodeCharge(localRedData->nodo);
			memcpy(
					localReductionRes + sizeof(char) + sizeof(int)
							+ (index * sizeof(rl_datos)), localRedData,
					sizeof(rl_datos));
		}
		return localReductionRes;
	} else {
		perror("\nTODAS LAS TRANSFORMACIONES DEBEN TERMINAR ANTES DE EMPEZAR LAS REDUCCIONES LOCALES.");
		return NULL;
	}

}
