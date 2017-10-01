/*
 * yamaStateTable.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "yamaStateTable.h"

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
} rowStateTable;

typedef struct{
	int node_id;
	int tasks_in_progress;
	int tasts_done;
	int considered;
	int availability;
}t_nodeStateTable;
*/

int findNode(Yama* yama, int node_id){
	int index = 0;
	if(!list_is_empty(yama->node_state_table)){
		for(index = 0; index < list_size(yama->node_state_table); index++){
				t_nodeStateTable* node = list_get(yama->node_state_table, index);
				if(node->node_id == node_id){
					return index;
				}
			}
	}
	return -1;
}

void viewNodeTable(Yama* yama){
	int index = 0;
	for(index = 0; index < list_size(yama->node_state_table); index++){
		t_nodeStateTable* node = list_get(yama->node_state_table, index);
	}
}

void updateNodeList(Yama* yama, void* fsInfo){
	t_header* head = malloc(sizeof(t_header));
	memcpy(head, fsInfo, sizeof(t_header));

	void* nodes = malloc(head->msg_size);
	memcpy(nodes, fsInfo+sizeof(t_header), head->msg_size);

	int index = 0;
	for(index = 0; index < head->msg_size; index += sizeof(block)){
		block* b = malloc(sizeof(block));
		memcpy(b, nodes+index, sizeof(block));
		int position = findNode(yama, b->node);
		if(position == -1){
			t_nodeStateTable* node = malloc(sizeof(t_nodeStateTable));
			node->availability = config_get_int_value(yama->config, "NODE_AVAIL");
			node->considered = 0;
			node->node_id = b->node;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			list_add(yama->node_state_table, node);
		}
	}
}


void viewStateTable(Yama* yama){
	t_list* stateTable = yama->state_table;
	printf("\nLa cantidad de entradas de la tabla es: %d\n", list_size(stateTable));
	int i = 0;
	while( i < list_size(stateTable) ){
		rowStateTable* row = list_get(stateTable, i);
		i++;
		printf("\nRow:\nJob=%d - Master=%d - Node=%d - Block=%d - Oper=%c - Temp=%s - Status=%d\n",
				row->job, row->master, row->node, row->block, row->op, row->tmp, row->status);
	}
}

int addToStatusTable(Yama* yama, int master, void* package){
	t_header head;
	memcpy(&head, package, sizeof(t_header));

	int nblocks = head.msg_size/sizeof(tr_datos);
	int added = sizeof(t_header);
	int i = 0;
	for(i = 0; i < nblocks; i++){
		tr_datos block;
		memcpy(&block, package+added, sizeof(tr_datos));
		added += sizeof(tr_datos);

		rowStateTable* row = malloc(sizeof(rowStateTable));
		row->block = block.bloque;
		row->job = 1;
		row->master = master;
		row->node = block.nodo;
		row->op = head.op;
		row->status = 1;
		strcpy(row->tmp, block.tr_tmp);

		list_add(yama->state_table, row);
	}
	return 1;
}
