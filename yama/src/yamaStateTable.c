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
} rowStateTable;*/

void viewStateTable(Yama* yama){
	t_list* stateTable = yama->stateTable;
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

		list_add(yama->stateTable, row);
	}
	return 1;
}
