/*
 * yamaStateTable.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "headers/yamaStateTable.h"

/*
 * typedef struct tr_datos {
 int nodo;
 char ip[15];
 long port;
 int bloque;
 int tamanio;
 char tr_tmp[28];
 } tr_datos;

 typedef struct {
 int job;
 int master;
 int node;
 int block;
 char op;
 char tmp[28];
 int status;
 } elem_tabla_estados;

 typedef struct{
 int node_id;
 int tasks_in_progress;
 int tasts_done;
 int considered;
 int availability;
 }elem_tabla_nodos;
 */

int findNode(Yama* yama, int node_id) {
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

void viewNodeTable(Yama* yama) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_nodos* node = list_get(yama->tabla_estados, index);
	}
}

void updateNodeList(void* fsInfo) {
	int* size = malloc(sizeof(int));
	memcpy(size, fsInfo, sizeof(int));

	void* nodes = malloc(*size);
	memcpy(nodes, fsInfo + sizeof(int), *size);

	int index = 0;
	for (index = 0; index < *size; index += sizeof(block)) {
		block* b = malloc(sizeof(block));
		memcpy(b, nodes + index, sizeof(block));
		int position = findNode(yama, b->node1);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = 0;
			node->considered = 0;
			node->node_id = b->node1;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			list_add(yama->tabla_nodos, node);
		}

		position = findNode(yama, b->node2);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = 0;
			node->considered = 0;
			node->node_id = b->node2;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			list_add(yama->tabla_nodos, node);
		}
	}
}

void viewStateTable(Yama* yama) {
	t_list* stateTable = yama->tabla_estados;
	printf("\nLa cantidad de entradas de la tabla es: %d\n",
			list_size(stateTable));
	int i = 0;
	while (i < list_size(stateTable)) {
		elem_tabla_estados* row = list_get(stateTable, i);
		i++;
		printf(
				"\nRow:\nJob=%d - Master=%d - Node=%d - Block=%d - Oper=%c - Temp=%s - Status=%d\n",
				row->job, row->master, row->node, row->block, row->op, row->tmp,
				row->status);
	}
}

int addToStatusTable(Yama* yama, int master, void* package) {
	t_header head;
	memcpy(&head, package, sizeof(t_header));

	int nblocks = head.msg_size / sizeof(tr_datos);
	int added = sizeof(t_header);
	int i = 0;
	for (i = 0; i < nblocks; i++) {
		tr_datos block;
		memcpy(&block, package + added, sizeof(tr_datos));
		added += sizeof(tr_datos);

		elem_tabla_estados* row = malloc(sizeof(elem_tabla_estados));
		row->block = block.bloque;
		row->job = 1;
		row->master = master;
		row->node = block.nodo;
		row->op = head.op;
		row->status = 1;
		strcpy(row->tmp, block.tr_tmp);

		list_add(yama->tabla_estados, row);
	}
	return 1;
}
