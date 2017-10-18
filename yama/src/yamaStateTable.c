/*
 * yamaStateTable.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "headers/yamaStateTable.h"

void setInStatusTable(char* tr_tmp, int nodo, char op, int master) {
	elem_tabla_estados* elemStatus = malloc(sizeof(elem_tabla_estados));
	elemStatus->block = getBlockId(tr_tmp);
	elemStatus->job = yama->jobs;
	elemStatus->master = master;
	elemStatus->node = nodo;
	elemStatus->op = op;
	elemStatus->status = 'P';
	strcpy(elemStatus->tmp, tr_tmp);

	updateStatusTable(elemStatus);
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
		printf("\nnodo id: %d, avail: %d, in progres: %d, done: %d\n",
				node->node_id, node->availability, node->tasks_in_progress,
				node->tasts_done);
	}
}

void viewPlannedTable() {
	printf(
			"\nLa cantidad de entradas de la tabla de tareas planificadas es: %d\n",
			list_size(yama->tabla_planificados));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_planificados); index++) {
		elem_tabla_planificados* planed = list_get(yama->tabla_planificados,
				index);
		printf("\nmaster: %d, node: %d, bloque: %d, temporal: %s\n",
				planed->master, planed->data->nodo, planed->data->bloque,
				planed->data->tr_tmp);
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

void updateNodeList(char op, int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	switch (op) {
	case 'T':
		node->availability--;
		node->tasks_in_progress++;
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
				"\nRow:\nJob=%d - Master=%d - Node=%d - Block=%d - Oper=%c - Temp=%s - Status=%c\n",
				row->job, row->master, row->node, row->block, row->op, row->tmp,
				row->status);
	}
}

int findRow(int master, int node_id, int block, char op) {
	int index = 0;
	if (!list_is_empty(yama->tabla_estados)) {
		for (index = 0; index < list_size(yama->tabla_estados); index++) {
			elem_tabla_estados* node = list_get(yama->tabla_estados, index);
			if ((node->node == node_id) && (master == node->master)
					&& (block == node->block) && (op == node->op)) {
				return index;
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

void updateStatusTable(elem_tabla_estados* elem) {
	int index = findRow(elem->master, elem->node, elem->block, elem->op);
	elem_tabla_estados* row;
	if (index == -1) {
		row = malloc(sizeof(elem_tabla_estados));
		row->master = elem->master;
		row->op = elem->op;
		row->status = elem->status;
		row->job = elem->job;
		row->node = elem->node;
		row->block = elem->block;
		strcpy(row->tmp, elem->tmp);
		list_add(yama->tabla_estados, row);
	} else {
		row = list_get(yama->tabla_estados, index);
		row->op = elem->op;
		row->status = elem->status;
		list_replace(yama->tabla_estados, index, row);
	}
}
