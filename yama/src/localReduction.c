/*
 * localReduction.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"

t_list* findPlaned(int master){
	t_list* planed_list = list_create();
	int i = 0;
	for(i = 0; i < list_size(yama->tabla_planificados); i++){
		elem_tabla_planificados* elem = list_get(yama->tabla_planificados, i);
		if( elem->master == master){
			list_add(planed_list, elem->data);
		}
	}
	return planed_list;
}

/*typedef struct rl_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[28];
	char	rl_tmp[28];
}rl_datos;*/

void getLocalReductionTmpName(rl_datos* nodeData, int op, int blockId, int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);

	strcpy(nodeData->tr_tmp, name);
}

void* processLocalReduction(int master){
	t_list* planed = findPlaned(master);

	void* localReductionRes = malloc(sizeof(char)+sizeof(int)+sizeof(rl_datos)*list_size(planed));
	memcpy(localReductionRes, "L", sizeof(char));
	int size = list_size(planed);
	memcpy(localReductionRes+sizeof(char), &size, sizeof(int));

	int index = 0;
	for(index = 0; index < list_size(planed); index++){
		elem_tabla_planificados* elem = list_get(planed, index);
		rl_datos* localRedData = malloc(sizeof(rl_datos));
		localRedData->nodo = elem->data->nodo;
		localRedData->port = elem->data->port;
		strcpy(localRedData->ip, elem->data->ip);
		strcpy(localRedData->tr_tmp, elem->data->tr_tmp);
		getLocalReductionTmpName(localRedData, 'L', elem->data->bloque, master);

		memcpy(localReductionRes+sizeof(char)+sizeof(int)+(index*sizeof(rl_datos)), localRedData, sizeof(rl_datos));
	}
	return localReductionRes;
}
