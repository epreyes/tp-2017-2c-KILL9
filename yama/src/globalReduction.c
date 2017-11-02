/*
 * globalReduction.c
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"

void viewGlobalReductionResponse(void* response) {
	char* op = malloc(sizeof(char));
	memcpy(op, response, sizeof(char));
	int* bloques = malloc(sizeof(int));
	memcpy(bloques, response + sizeof(char), sizeof(int));
	printf("\nLa respuesta de la reduccion GLOBAL es (%c, %d, %d): ", *op,
			*bloques, (*bloques) * sizeof(rg_datos));
	int plus = sizeof(int) + sizeof(char);
	int i = 0;
	for (i = 0; i < *bloques; i++) {
		rg_datos* data = malloc(sizeof(rg_datos));
		memcpy(data, response + plus + (i * sizeof(rg_datos)),
				sizeof(rg_datos));
		printf(
				"\nNodo=%d - Ip=%s - Puerto=%d - RL_TMP=%s - RG_TMP=%s, Encargado=%c\n",
				data->nodo, data->ip, data->port, data->rl_tmp, data->rg_tmp,
				data->encargado);
	}
}

void deleteOfLocalReductionPlanedList(int items, int master) {
	int i = 0;
	for (i = 0; i < items; i++) {
		int j = 0;
		for (j = 0; j < list_size(yama->tabla_LR_planificados); j++) {
			elem_tabla_LR_planificados* elem = list_get(
					yama->tabla_LR_planificados, j);
			if (elem->master == master) {
				list_remove(yama->tabla_LR_planificados, j);
				break;
			}
		}

	}
}

t_list* findLocalReductionPlaned(int master) {
	int itemsToRemove = 0;
	t_list* planed_list = list_create();
	int i = 0;
	for (i = 0; i < list_size(yama->tabla_LR_planificados); i++) {
		elem_tabla_LR_planificados* elem = list_get(yama->tabla_LR_planificados,
				i);
		if (elem->master == master) {
			list_add(planed_list, elem->data);
			itemsToRemove++;
		}
	}
	deleteOfLocalReductionPlanedList(itemsToRemove, master);
	return planed_list;
}

void getGlobalReductionTmpName(rg_datos* nodeData, int op, int blockId,
		int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);
	//printf("\nEl nbombre creado es: %s (%d)\n", name, sizeof(name));

	strcpy(nodeData->rg_tmp, name);
}

int allLocalReductionProcesFinish(int master) {
	int response = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if (elem->master == master && elem->op == 'L') {
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

rl_datos* getLastChargedNode() {
	viewNodeTable();
	int min = 10000;
	rl_datos* minNode = list_get(yama->tabla_nodos, 0);
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		if (min > elem->tasks_in_progress) {
			min = elem->tasks_in_progress;
			minNode = elem;
		}
	}
	return minNode;
}

void* processGlobalReduction(int master) {

	void* globalReductionRes;

	if (allLocalReductionProcesFinish(master)) {
		t_list* planed = findLocalReductionPlaned(master);

		globalReductionRes = malloc(
					sizeof(char) + sizeof(int) + sizeof(rg_datos) * list_size(planed));

		memcpy(globalReductionRes, "G", sizeof(char));
		int size = list_size(planed);
		memcpy(globalReductionRes + sizeof(char), &size, sizeof(int));

		rl_datos* enchargeNode = getLastChargedNode();
		int enchargeSeted = 0;

		int index = 0;
		for (index = 0; index < list_size(planed); index++) {
			rl_datos* elem = list_get(planed, index);
			rg_datos* globalRedData = malloc(sizeof(rg_datos));
			globalRedData->nodo = elem->nodo;
			globalRedData->port = elem->port;
			strcpy(globalRedData->ip, elem->ip);
			strcpy(globalRedData->rl_tmp, elem->rl_tmp);

			if ( enchargeSeted == 0 && globalRedData->nodo == enchargeNode->nodo) {
				printf("\nMetiendo como encargado al nodo %d (%d) \n", enchargeNode->nodo, globalRedData->nodo);

				getGlobalReductionTmpName(globalRedData, 'G',
						0, master);
				globalRedData->encargado = 'T';
				setInStatusTable('G', master, globalRedData->nodo, 0,
						globalRedData->rg_tmp, 0);
				increaseNodeCharge(globalRedData->nodo);
				addToGlobalReductionPlanedTable(master, globalRedData);
				enchargeSeted = 1;
			} else {
				getGlobalReductionTmpName(globalRedData, 'G',
										0, master);
				globalRedData->encargado = 'F';
			}

			printf("\nVa a copiar en el buffer: Encargado %c, tmp: %s\n", globalRedData->encargado, globalRedData->rg_tmp);

			memcpy(
					globalReductionRes + sizeof(char) + sizeof(int)
							+ (index * sizeof(rg_datos)), globalRedData,
					sizeof(rg_datos));
		}
		return globalReductionRes;
	} else {
		perror(
				"\nTODAS LAS REDUCCIONES LOCALES DEBEN TERMINAR ANTES DE EMPEZAR LA REDUCCION GLOBAL.");
		globalReductionRes = malloc(sizeof(char));
		memcpy(globalReductionRes, "X", sizeof(char));
		return globalReductionRes;
	}
}
