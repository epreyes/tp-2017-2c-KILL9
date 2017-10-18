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
	printf("\nLa respuesta de la Global local es (%c, %d, %d): ", *op,
			*bloques, (*bloques) * sizeof(rg_datos));
	int plus = sizeof(int) + sizeof(char);
	int i = 0;
	for (i = 0; i < *bloques; i++) {
		rg_datos* data = malloc(sizeof(rg_datos));
		memcpy(data, response + plus + (i * sizeof(rg_datos)),
				sizeof(rg_datos));
		printf("\nNodo=%d - Ip=%s - Puerto=%d - RL_TMP=%s - RG_TMP=%s, Encargado=%c\n",
				data->nodo, data->ip, data->port, data->rl_tmp, data->rg_tmp, data->encargado);
	}
}

t_list* findLocalReductionPlaned(int master) {
	t_list* planed_list = list_create();
	int i = 0;
	for (i = 0; i < list_size(yama->tabla_LR_planificados); i++) {
		elem_tabla_planificados* elem = list_get(yama->tabla_LR_planificados,
				i);
		if (elem->master == master) {
			list_add(planed_list, elem->data);
		}
	}
	return planed_list;
}

void getGlobalReductionTmpName(rg_datos* nodeData, int op, int blockId,
		int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);

	strcpy(nodeData->rg_tmp, name);
}

void* processGlobalReduction(int master) {
	t_list* planed = findLocalReductionPlaned(master);

	void* globalReductionRes = malloc(
			sizeof(char) + sizeof(int) + sizeof(rg_datos) * list_size(planed));
	memcpy(globalReductionRes, "G", sizeof(char));
	int size = list_size(planed);
	memcpy(globalReductionRes + sizeof(char), &size, sizeof(int));

	int index = 0;
	for (index = 0; index < list_size(planed); index++) {
		rl_datos* elem = list_get(planed, index);
		rg_datos* globalRedData = malloc(sizeof(rg_datos));
		globalRedData->nodo = elem->nodo;
		globalRedData->port = elem->port;
		strcpy(globalRedData->ip, elem->ip);
		strcpy(globalRedData->rl_tmp, elem->rl_tmp);
		getGlobalReductionTmpName(globalRedData, 'G', getBlockId(elem->rl_tmp),
				master);
		setInStatusTable(globalRedData->rg_tmp, globalRedData->nodo, 'G',
				master);
		globalRedData->encargado = 'N';
		if( globalRedData->nodo == 2){
			globalRedData->encargado = 'Y';
		}
		memcpy(
				globalReductionRes + sizeof(char) + sizeof(int)
						+ (index * sizeof(rl_datos)), globalRedData,
				sizeof(rl_datos));
	}
	return globalReductionRes;
}
