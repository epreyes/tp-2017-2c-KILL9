/*
 * yamaOperations.c
 *
 *  Created on: 14/09/2017
 *      Author: utnso
 */
#include "yamaOperations.h"
#include "yamaFS.h"
#include "yamaUtils.h"

/*-----------------------------------------------------------------------------------*/

void* processTransformation(t_header* head, void* fsInfo, int master) {

	int totalBlocksRecived = head->msg_size / (sizeof(block));
	int totalSizeToSend = totalBlocksRecived * sizeof(tr_datos);
	head->op = 'T';
	head->msg_size = totalSizeToSend;

	t_list* listToOrder = list_create();

	int index = 0;
	for (index = 0; index < totalBlocksRecived; index++) {

		block blockRecived;
		memcpy(&blockRecived, fsInfo + (index * sizeof(block)), sizeof(block));

		tr_datos* rsBlock = malloc(sizeof(tr_datos));
		rsBlock->nodo = blockRecived.node;
		strcpy(rsBlock->ip, blockRecived.node_ip);
		rsBlock->port = blockRecived.node_port;
		rsBlock->bloque = blockRecived.node_block;
		rsBlock->tamanio = blockRecived.end_block;
		strcpy(rsBlock->tr_tmp, getTmpName(head, rsBlock->bloque, master));
		list_add(listToOrder, rsBlock);
	}

	return sortTransformationResponse(listToOrder, totalSizeToSend);
}

int transformation(Yama* yama, t_header head, int master, void* fsInfo) {
	int response = 0;

	log_trace(yama->log, "Doing transformation...");

	void* masterRS = processTransformation(&head, fsInfo, master);

	void* package = buildPackage(masterRS, head);

	int sizepack = sizeof(t_header) + head.msg_size;

	if( sendPackage(yama, master, package, sizepack) > 0 ){
		response = addToStatusTable(yama, master, package);
		viewStateTable(yama);
	}
	return response;
}

void* processLocalReduction(t_header* head, void* fsInfo, int master) {
	int totalBlocksRecived = head->msg_size / (sizeof(block));
	int totalSizeToSend = totalBlocksRecived * sizeof(tr_datos);
	head->op = 'T';
	head->msg_size = totalSizeToSend;

	t_list* listToOrder = list_create();

	int index = 0;
	for (index = 0; index < totalBlocksRecived; index++) {

		block blockRecived;
		memcpy(&blockRecived, fsInfo + (index * sizeof(block)), sizeof(block));

		tr_datos* rsBlock = malloc(sizeof(tr_datos));
		rsBlock->nodo = blockRecived.node;
		strcpy(rsBlock->ip, blockRecived.node_ip);
		rsBlock->port = blockRecived.node_port;
		rsBlock->bloque = blockRecived.node_block;
		rsBlock->tamanio = blockRecived.end_block;
		strcpy(rsBlock->tr_tmp, getTmpName(head, rsBlock->bloque, master));
		list_add(listToOrder, rsBlock);
	}

	return sortLocalReductionResponse(listToOrder, totalSizeToSend);
}

int localReduction(Yama* yama, t_header head, int master, void* fsInfo) {
	log_trace(yama->log, "Doing Local Reduction...");

	void* masterRS = processLocalReduction(&head, fsInfo, master);

	t_header headSend = head;
	void* package = malloc(sizeof(t_header) + head.msg_size);

	printf("\nTamaño mensaje : %d \nCantidad de elementos en la lista: %d\n",
			head.msg_size, head.msg_size / sizeof(tr_datos));
	memcpy(package, &headSend, sizeof(t_header));
	memcpy(package + sizeof(t_header), masterRS, head.msg_size);
	int sizepack = sizeof(t_header) + head.msg_size;
	return send(master, package, sizepack, 0);
	return 1;
}

int globalReduction(Yama* yama, t_header head, int master, void* fsInfo) {
	log_trace(yama->log, "Doing Global Reduction...");
	return 1;
}

int finalStore(Yama* yama, t_header head, int master, void* fsInfo) {
	log_trace(yama->log, "Doing Final Store...");
	return 1;
}

