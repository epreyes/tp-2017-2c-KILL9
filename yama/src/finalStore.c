/*
 * finalStore.c
 *
 *  Created on: 19/10/2017
 *      Author: utnso
 */
#include "headers/finalStore.h"

int allGlobalReductionProcesFinish(int master) {
	int response = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if (elem->master == master && elem->op == 'G') {
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
			list_add(planed_list, elem->data);
			itemsToRemove++;
		}
	}

	deleteOfGlobalReductionPlanedList(itemsToRemove, master);

	return planed_list;
}

void viewFinalStoreResponse( void* response ){
	char op;
	memcpy(&op, response, sizeof(char));

	int blocks;
	memcpy(&blocks, response+sizeof(char), sizeof(int));

	void* data = malloc(sizeof(af_datos)*blocks);
	memcpy(data, response+sizeof(char)+sizeof(int), sizeof(af_datos)*blocks);

	int index = 0;
	printf("\nOperacion %c, Bloques %d\n", op, blocks);
	for(index = 0; index < blocks; index++){
		af_datos* afdata = malloc(sizeof(af_datos));
		memcpy(afdata, data+(index*sizeof(af_datos)), sizeof(af_datos));

		printf( "\nnodo=%d, ip=%s, puerto=%d, rgtmp=%s\n", afdata->nodo, afdata->ip, afdata->port, afdata->rg_tmp);
	}
}

/*typedef struct {
	int		nodo;
	char	ip[16];
	int 	port;
	char	rg_tmp[24];
}af_datos;*/
void* processFinalStore(int master) {
	if (allGlobalReductionProcesFinish(master)) {
		t_list* planed = findGlobalReductionPlaned(master);

		int fsSize = sizeof(char)*41+sizeof(int)*2;

		void* finalStoreRes = malloc(
				sizeof(char) + sizeof(int)
						+ fsSize * list_size(planed));

		memcpy(finalStoreRes, "S", sizeof(char));
		int size = list_size(planed);

		memcpy(finalStoreRes + sizeof(char), &size, sizeof(int));

		void* fsdata = malloc(fsSize * list_size(planed));

		int index = 0;
		for (index = 0; index < list_size(planed); index++) {
			rg_datos* elem = list_get(planed, index);
			af_datos* finalStoreData = malloc(sizeof(af_datos));
			finalStoreData->nodo = elem->nodo;
			strcpy(finalStoreData->ip, elem->ip);
			finalStoreData->port = elem->port;
			strcpy(finalStoreData->rg_tmp, elem->rg_tmp);

			setInStatusTable('S', master, finalStoreData->nodo, 0,
					finalStoreData->rg_tmp, 0);
			increaseNodeCharge(finalStoreData->nodo);

			/*memcpy(
					finalStoreRes + sizeof(char) + sizeof(int)
							+ (index * sizeof(af_datos)), finalStoreData,
					sizeof(af_datos));*/
			memcpy(fsdata+(index*fsSize), &(finalStoreData->nodo), sizeof(int));
			memcpy(fsdata+(index*fsSize)+sizeof(int), finalStoreData->ip, sizeof(char)*16);
			memcpy(fsdata+(index*fsSize)+sizeof(int)+sizeof(char)*16, &(finalStoreData->port), sizeof(int));
			memcpy(fsdata+(index*fsSize)+sizeof(int)+sizeof(char)*16+sizeof(int), finalStoreData->rg_tmp, 24*sizeof(char));
		}
		memcpy(finalStoreRes+sizeof(char)+sizeof(int), fsdata, fsSize * list_size(planed));
		return finalStoreRes;
	} else {
		perror(
				"\nLA REDUCCION GLOBAL DEBE TERMINAR ANTES DE EMPEZAR EL ALMACENAMIENTO FINAL.");
		return NULL;
	}
}
