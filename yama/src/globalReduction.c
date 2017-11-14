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

	int totalPlus = sizeof(int) + sizeof(char) * 16 + sizeof(int)
			+ sizeof(char) * 28 + sizeof(char) * 24 + sizeof(char);

	int plus = sizeof(int) + sizeof(char);
	int i = 0;
	for (i = 0; i < *bloques; i++) {
		rg_datos* data = malloc(sizeof(rg_datos));
		memcpy(&(data->nodo), response + (totalPlus * i) + plus, sizeof(int));

		strncpy(data->ip, response + (totalPlus * i) + plus + sizeof(int),
				sizeof(char) * 16);

		memcpy(&(data->port),
				response + (totalPlus * i) + plus + sizeof(int)
						+ sizeof(char) * 16, sizeof(int));

		strncpy(data->rl_tmp,
				response + (totalPlus * i) + plus + sizeof(int)
						+ sizeof(char) * 16 + sizeof(int), sizeof(char) * 28);

		strncpy(data->rg_tmp,
				response + (totalPlus * i) + plus + sizeof(int)
						+ sizeof(char) * 16 + sizeof(int) + sizeof(char) * 28,
				sizeof(char) * 24);

		memcpy(&(data->encargado),
				response + (totalPlus * i) + plus + sizeof(int)
						+ sizeof(char) * 16 + sizeof(int) + sizeof(char) * 28
						+ sizeof(char) * 24, sizeof(char));

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
			list_add(planed_list, elem);
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
	asprintf(&name, "%s%ld-RG-M%03d", "/tmp/", timestamp, masterId);

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

int getLastChargedNode(t_list* planed) {

	elem_tabla_planificados* data = list_get(planed, 0);
	rl_datos* minNode = malloc(sizeof(rl_datos));
	memcpy(minNode, data->data, sizeof(rl_datos));

	if (list_size(planed) > 1) {
		int index = 1;
		for (index = 1; index < list_size(planed); index++) {
			//traigo el primer nodo planificado para este job.
			elem_tabla_nodos* elemNodeMin = list_get(yama->tabla_nodos, findNode( minNode->nodo ));

			//tomo el segundo elemento planificado para compararlo contra el primero.
			elem_tabla_planificados* elemPlaned = list_get(planed, index);
			rl_datos* elem = malloc(sizeof(rl_datos));
			memcpy(elem, elemPlaned->data, sizeof(rl_datos));

			//traigo el nodo de ese elemento planificado.
			elem_tabla_nodos* nextNodeElem = list_get(yama->tabla_nodos, findNode( elem->nodo ));

			//comparo las cargas actuales.
			if( elemNodeMin->tasks_in_progress > nextNodeElem->tasks_in_progress){
				minNode = elem;
			}
		}

	}
	return minNode->nodo;
}

void* processGlobalReduction(int master) {

	void* globalReductionRes;

	if (allLocalReductionProcesFinish(master)) {
		t_list* planed = findLocalReductionPlaned(master);

		int tamData = sizeof(int) * 2 + (sizeof(char) * 69);

		globalReductionRes = malloc(
				sizeof(char) + sizeof(int) + tamData * list_size(planed));

		void* dataGR = malloc(tamData * list_size(planed));

		memcpy(globalReductionRes, "G", sizeof(char));
		int size = list_size(planed);
		memcpy(globalReductionRes + sizeof(char), &size, sizeof(int));

		int enchargeNode = getLastChargedNode(planed);
		int enchargeSeted = 0;

		int index = 0;
		for (index = 0; index < list_size(planed); index++) {

			elem_tabla_planificados* data = list_get(planed, index);
			rl_datos* elem = malloc(sizeof(rl_datos));
			memcpy(elem, data->data, sizeof(rl_datos));
			rg_datos* globalRedData = malloc(sizeof(rg_datos));
			globalRedData->nodo = elem->nodo;
			globalRedData->port = elem->port;
			strcpy(globalRedData->ip, elem->ip);
			strcpy(globalRedData->rl_tmp, elem->rl_tmp);

			if (enchargeSeted == 0
					&& globalRedData->nodo == enchargeNode) {

				getGlobalReductionTmpName(globalRedData, 'G', 0, master);
				globalRedData->encargado = 'T';
				setInStatusTable('G', master, globalRedData->nodo, 0,
						globalRedData->rg_tmp, 0, data->fileName);
				increaseNodeCharge(globalRedData->nodo);
				addToGlobalReductionPlanedTable(master, globalRedData, data->fileName);
				enchargeSeted = 1;
			} else {
				getGlobalReductionTmpName(globalRedData, 'G', 0, master);
				globalRedData->encargado = 'F';
			}

			memcpy(dataGR + (index * tamData), &(globalRedData->nodo),
					sizeof(int));
			memcpy(dataGR + (index * tamData) + sizeof(int), globalRedData->ip,
					16 * sizeof(char));
			memcpy(dataGR + (index * tamData) + sizeof(int) + 16 * sizeof(char),
					&(globalRedData->port), sizeof(int));
			memcpy(
					dataGR + (index * tamData) + sizeof(int) + 16 * sizeof(char)
							+ sizeof(int), globalRedData->rl_tmp,
					28 * sizeof(char));
			memcpy(
					dataGR + (index * tamData) + sizeof(int) + 16 * sizeof(char)
							+ sizeof(int) + 28 * sizeof(char),
					globalRedData->rg_tmp, 24 * sizeof(char));
			memcpy(
					dataGR + (index * tamData) + sizeof(int) + 16 * sizeof(char)
							+ sizeof(int) + 28 * sizeof(char)
							+ 24 * sizeof(char), &(globalRedData->encargado),
					sizeof(char));

		}
		memcpy(globalReductionRes + sizeof(char) + sizeof(int), dataGR,
				tamData * list_size(planed));
		return globalReductionRes;
	} else {
		log_warning(yama->log,
				"Todas las reducciones locales deben terminar antes de empezar la reducción global.");
		globalReductionRes = malloc(sizeof(char));
		memcpy(globalReductionRes, "X", sizeof(char));
		return globalReductionRes;
	}
}
