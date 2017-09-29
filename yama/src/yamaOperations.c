/*
 * yamaOperations.c
 *
 *  Created on: 14/09/2017
 *      Author: utnso
 */
#include "yamaOperations.h"
#include "yamaStateTable.h"

/*-----------------------------------------------------------------------------------*/

void* processTransformation(Yama* yama, t_fileInfo* fsInfo, int master) {
	t_list*  nodeList = list_create();
	nodeList = buildTransformationResponseNodeList(yama, fsInfo, master);

	//doPlanning();

	return sortTransformationResponse(nodeList);
}

int transformation(Yama* yama, t_header head, int master, void* payload) {
	int response = 0;

	return response;
}

void* processLocalReduction(t_header* head, void* fsInfo, int master) {


	return 1;
}

int localReduction(Yama* yama, t_header head, int master, void* fsInfo) {
	log_trace(yama->log, "Doing Local Reduction...");

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

/*------------------------------------------------------------------------------------------*/

char* getTmpName(int op, int blockId, int masterId) {
	char name[28];
	long timestamp = current_timestamp();
	snprintf(name, sizeof(name), "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp,
			op, masterId, blockId);

	return name;
}

bool compareTransformationBlocks(void* b1, void* b2) {
	bool response;
	if (((tr_datos*) b1)->nodo <= ((tr_datos*) b2)->nodo) {
		response = 1;
	} else {
		response = 0;
	}
	return response;
}

t_list* buildTransformationResponseNodeList(Yama* yama, t_fileInfo* fsInfo,
		int master) {

	void* info = malloc(fsInfo->sizeInfo);
	memcpy(info, fsInfo->info, fsInfo->sizeInfo);

	t_list* nodeList = list_create();

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block* blockInfo = malloc(sizeof(block));
		memcpy(blockInfo, info + (index * sizeof(block)), sizeof(block));
		tr_datos* nodeData = buildMasterResponseBlock('T', blockInfo, master);
		list_add(nodeList, nodeData);
	}
	return nodeList;
}

void* sortTransformationResponse(t_list* buffer) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	t_header head;
	head.msg_size = sizeof(tr_datos)*list_size(buffer);
	head.op = 'T';

	void* sortedBuffer = malloc( sizeof(t_header) + head.msg_size );
	memcpy(sortedBuffer, &head, sizeof(t_header));

	int index = 0;
	for (index = 0; index < list_size(buffer); index++) {
		tr_datos* data = (tr_datos*) list_get(buffer, index);
		memcpy(sortedBuffer + sizeof(t_header) + (index * sizeof(tr_datos)), data,
				sizeof(tr_datos));
	}
	return sortedBuffer;
}

tr_datos* buildMasterResponseBlock(int op, block* blockRecived, int master) {
	tr_datos* rsBlock = malloc(sizeof(tr_datos));
	rsBlock->nodo = blockRecived->node;
	strcpy(rsBlock->ip, blockRecived->node_ip);
	rsBlock->port = blockRecived->node_port;
	rsBlock->bloque = blockRecived->node_block;
	rsBlock->tamanio = blockRecived->end_block;
	strcpy(rsBlock->tr_tmp, getTmpName(op, rsBlock->bloque, master));

	return rsBlock;
}

int sendPackage(Yama* yama, int master, void* package, int sizepack) {
	int bytesSent = send(master, package, sizepack, 0);
	if (bytesSent > 0) {
		//addToStateTable();
		char message[30];
		snprintf(message, sizeof(message), "%d bytes sent to master id %d",
				bytesSent, master);
		log_trace(yama->log, message);
	} else {
		perror("Send response to master.");
	}
	return bytesSent;
}
