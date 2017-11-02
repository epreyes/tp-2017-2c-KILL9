/*
 * yamaStateTable.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "headers/yamaStateTable.h"

int existInStatusTable(int job, char op, int node){
	if (!list_is_empty(yama->tabla_estados)) {
		int index = 0;
			for (index = 0; index < list_size(yama->tabla_estados); index++) {
				elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
				if ( elem->job == job && elem->node == node && elem->op == op && elem->status == 'P' ) {
					return 1;
				}
			}
			return 0;
		}
	else{
		return 0;
	}
}

void setInStatusTable(char op, int master, int nodo, int bloque, char* tmpName,
		int nodeBlock) {
	elem_tabla_estados* elemStatus = malloc(sizeof(elem_tabla_estados));
	elemStatus->block = bloque;
	elemStatus->job = yama->jobs + master;
	elemStatus->master = master;
	elemStatus->node = nodo;
	elemStatus->node_block = nodeBlock;
	elemStatus->op = op;
	elemStatus->status = 'P';
	strcpy(elemStatus->tmp, tmpName);

	addNewRowStatusTable(elemStatus);
}

int findNode(int node_id) {
	int index = 0;
	if (!list_is_empty(yama->tabla_nodos)) {
		for (index = 0; index < list_size(yama->tabla_nodos); index++) {
			elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
			if (node->node_id == node_id) {
				return index;
			}
		}
	}
	return -1;
}

void viewNodeTable() {
	printf("\nLa cantidad de entradas de la tabla de nodos es: %d\n",
			list_size(yama->tabla_nodos));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
		printf("nodo id: %d, avail: %d, in progres: %d, done: %d\n",
				node->node_id, node->availability, node->tasks_in_progress,
				node->tasts_done);
	}
}

void viewPlannedTable() {
	printf(
			"\nLa cantidad de entradas de la tabla de tareas planificadas es: %d\n",
			list_size(yama->tabla_T_planificados));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_T_planificados); index++) {
		elem_tabla_planificados* planed = list_get(yama->tabla_T_planificados,
				index);
		printf("\nmaster: %d, node: %d, bloque: %d, temporal: %s\n",
				planed->master, planed->data->nodo, planed->data->bloque,
				planed->data->tr_tmp);
	}
}

void viewFileInfo() {
	printf(
			"\nLa cantidad de entradas de la tabla de informacion de archivos: %d\n",
			list_size(yama->tabla_info_archivos));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
		elem_info_archivo* info = list_get(yama->tabla_info_archivos, index);
		printf("\nBloques=%d - Filename=%s - Sizeinfo=%d\n", info->blocks,
				info->filename, info->sizeInfo);
	}
}

void viewLRPlannedTable() {
	printf(
			"\nLa cantidad de entradas de la tabla de reducciones locales es: %d\n",
			list_size(yama->tabla_LR_planificados));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_LR_planificados); index++) {
		elem_tabla_LR_planificados* planed = list_get(
				yama->tabla_LR_planificados, index);
		printf(
				"\nmaster: %d, node: %d, ip: %s, port: %d, TRtemporal: %s, LRtemporal: %s\n",
				planed->master, planed->data->nodo, planed->data->ip,
				planed->data->port, planed->data->tr_tmp, planed->data->rl_tmp);
	}
}

void addToNodeList(void* fsInfo) {
	int* size = malloc(sizeof(int));
	memcpy(size, fsInfo, sizeof(int));

	void* nodes = malloc(*size);
	memcpy(nodes, fsInfo + sizeof(int), *size);

	int index = 0;
	for (index = 0; index < *size; index += sizeof(block_info)) {
		block_info* b = malloc(sizeof(block_info));
		memcpy(b, nodes + index, sizeof(block_info));
		int position = findNode(b->node1);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = config_get_int_value(yama->config,
					"NODE_AVAIL");
			node->considered = 0;
			node->node_id = b->node1;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			node->errors = 0;
			list_add(yama->tabla_nodos, node);
		}

		position = findNode(b->node2);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = config_get_int_value(yama->config,
					"NODE_AVAIL");
			node->considered = 0;
			node->node_id = b->node2;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			list_add(yama->tabla_nodos, node);
		}
	}
}

void increaseNodeCharge(int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	node->availability--;
	node->tasks_in_progress++;
	list_replace(yama->tabla_nodos, index, node);
}

void decreaseNodeCharge(int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	node->availability++;
	node->tasks_in_progress--;
	node->tasts_done++;
	list_replace(yama->tabla_nodos, index, node);
}

void updateNodeList(char op, int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	switch (op) {
	case 'T':
		node->availability++;
		node->tasks_in_progress--;
		node->tasts_done++;
		break;
	case 'L':
	case 'G':
		node->tasts_done++;
		break;
	case 'O':
		node->availability++;
		node->tasks_in_progress--;
		node->tasts_done++;
	}
}

void viewStateTable() {
	t_list* stateTable = yama->tabla_estados;
	printf("\nLa cantidad de entradas de la tabla es: %d\n",
			list_size(stateTable));
	int i = 0;
	while (i < list_size(stateTable)) {
		elem_tabla_estados* row = list_get(stateTable, i);
		i++;
		printf(
				"Row:\nJob=%d - Master=%d - Block=%d - Node=%d - Node_block=%d - Oper=%c - Temp=%s - Status=%c\n",
				row->job, row->master, row->block, row->node, row->node_block,
				row->op, row->tmp, row->status);
	}
}

int findInProcessTasks(int master, int node_id, int block, char op) {
	int index = 0;
	if (!list_is_empty(yama->tabla_estados)) {
		for (index = 0; index < list_size(yama->tabla_estados); index++) {
			elem_tabla_estados* node = list_get(yama->tabla_estados, index);

			if (op == 'T') {
				if ((node->node == node_id) && (master == node->master)
						&& (block == node->node_block) && (op == node->op)
						&& (node->status == 'P')) {
					return index;
				}
			}
			else {
				if ((node->node == node_id) && (master == node->master)
						&& (op == node->op) && (node->status == 'P')) {
					return index;
				}
			}

		}
	}
	return -1;
}

int getBlockId(char* tmpName) {
	char block_code[3];
	strncpy(block_code, tmpName + 24, 3);
	int code = 0;
	sscanf(block_code, "%d", &code);
	return code;
}

void addNewRowStatusTable(elem_tabla_estados* elem) {
	elem_tabla_estados* row;

	row = malloc(sizeof(elem_tabla_estados));
	row->master = elem->master;
	row->op = elem->op;
	row->status = elem->status;
	row->job = elem->job;
	row->node = elem->node;
	row->node_block = elem->node_block;
	row->block = elem->block;
	strcpy(row->tmp, elem->tmp);
	list_add(yama->tabla_estados, row);
}

void updateStatusTable(int master, char opCode, int node, int bloque,
		char status) {

	int index = findInProcessTasks(master, node, bloque, opCode);
	elem_tabla_estados* row;
	if (index > -1) {
		row = list_get(yama->tabla_estados, index);
		row->op = opCode;
		row->status = status;
		list_replace(yama->tabla_estados, index, row);
	} else {
		printf(
				"\nNO PUEDE actualizar master %d, opCode %c, nodo %d, bloque %d, estado %c\n",
				master, opCode, node, bloque, status);
	}
}

void addToTransformationPlanedTable(int master, tr_datos* nodeData) {
	elem_tabla_planificados* planed = malloc(sizeof(elem_tabla_planificados));
	planed->data = malloc(sizeof(tr_datos));
	memcpy(planed->data, nodeData, sizeof(tr_datos));
	planed->master = master;
	list_add(yama->tabla_T_planificados, planed);
}

void addToLocalReductionPlanedTable(int master, rl_datos* nodeData) {
	elem_tabla_LR_planificados* planed = malloc(
			sizeof(elem_tabla_LR_planificados));
	planed->data = malloc(sizeof(rl_datos));
	memcpy(planed->data, nodeData, sizeof(rl_datos));
	planed->master = master;
	list_add(yama->tabla_LR_planificados, planed);
}

void addToGlobalReductionPlanedTable(int master, rg_datos* nodeData) {
	elem_tabla_GR_planificados* planed = malloc(
			sizeof(elem_tabla_GR_planificados));
	planed->data = malloc(sizeof(rg_datos));
	memcpy(planed->data, nodeData, sizeof(rg_datos));
	planed->master = master;
	list_add(yama->tabla_GR_planificados, planed);
}

void updateTasksAborted(int master, int node, int codeOp) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if ((elem->node == node) && (elem->master == master)
				&& (elem->op == codeOp) && (elem->status == 'P')) {
			updateStatusTable(master, codeOp, node, elem->block, 'A');
		}
	}
}

t_list* getTaskFailed(int master, int node, int code) {
	int index = 0;
	t_list* tasksFails = list_create();
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if ((elem->node == node) && (elem->master == master)
				&& (elem->op == 'T') && (elem->status == 'P')) {
			list_add(tasksFails, elem);
			updateStatusTable(master, 'T', node, elem->block, 'E');
		}
	}
	return tasksFails;
}

block_info* findBlock(int block) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
		elem_info_archivo* elem = list_get(yama->tabla_info_archivos, index);
		void* info = malloc(elem->sizeInfo);
		memcpy(info, elem->info, elem->sizeInfo);
		int j = 0;
		for (j = 0; j < elem->blocks; j++) {
			block_info* blockInfo = malloc(sizeof(block_info));
			memcpy(blockInfo, info + (j * sizeof(block_info)),
					sizeof(block_info));
			if (blockInfo->block_id == block) {
				return blockInfo;
			}
		}
	}

	return NULL;
}

int getBlockOfFilie( nodo, bloque) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
		elem_info_archivo* elem = list_get(yama->tabla_info_archivos, index);
		void* info = malloc(elem->sizeInfo);
		memcpy(info, elem->info, elem->sizeInfo);

		int j = 0;
		for (j = 0; j < elem->blocks; j++) {
			block_info* blockInfo = malloc(sizeof(block_info));
			memcpy(blockInfo, info + (j * sizeof(block_info)),
					sizeof(block_info));
			if ((blockInfo->node1 == nodo && blockInfo->node1_block == bloque)
					|| (blockInfo->node2 == nodo
							&& blockInfo->node2_block == bloque)) {
				return blockInfo->block_id;
			}
		}

		free(info);
	}
	return -1;
}

