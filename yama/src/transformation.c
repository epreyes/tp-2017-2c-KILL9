/*
 * transformation.c
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#include "headers/transformation.h"
#include "headers/yamaFS.h"

void* processTransformation(int master) {
	/*me traigo la informacion del archivo.*/
	elem_info_archivo* fsInfo = getFileInfo(master);

	/*Creo una lista, que va a ser la respuesta que se le va a mandar al master, sin planificar.*/
	t_list* nodeList = list_create();
	nodeList = buildTransformationResponseNodeList(fsInfo, master);

	int i = 0;
	for (i = 0; i < list_size(nodeList); i++) {
		tr_datos* data = list_get(nodeList, i);
		printf("\nlueog de build transformation %d - %s - %d - %d - %d - %s\n", data->nodo, data->ip,
				data->port, data->tamanio, data->bloque, data->tr_tmp);
	}
	printf(
			"\n------------------------------------------------------------------------\n");

	return sortTransformationResponse(nodeList);
}

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo,
		int master) {

	void* info = malloc(fsInfo->sizeInfo);
	memcpy(info, fsInfo->info, fsInfo->sizeInfo);

	t_list* nodeList = list_create();

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block_info* blockInfo = malloc(sizeof(block_info));
		memcpy(blockInfo, info + (index * sizeof(block_info)),
				sizeof(block_info));

		tr_datos* nodeData = malloc(sizeof(tr_datos));
		nodeData->nodo = blockInfo->node1;
		strcpy(nodeData->ip, blockInfo->node1_ip);
		nodeData->port = blockInfo->node1_port;
		nodeData->bloque = blockInfo->node1_block;
		nodeData->tamanio = blockInfo->end_block;
		getTmpName(nodeData, 'T', nodeData->bloque, master);
		list_add(nodeList, nodeData);

		tr_datos* nodeData2 = malloc(sizeof(tr_datos));
		nodeData2->nodo = blockInfo->node2;
		strcpy(nodeData2->ip, blockInfo->node2_ip);
		nodeData2->port = blockInfo->node2_port;
		nodeData2->bloque = blockInfo->node2_block;
		nodeData2->tamanio = blockInfo->end_block;
		getTmpName(nodeData2, 'T', nodeData2->bloque, master);
		list_add(nodeList, nodeData2);

		free(blockInfo);
	}

	return nodeList;
}

void* sortTransformationResponse(t_list* buffer) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	printf(
			"\n--------------------------Ordenada-------------------------------\n");
	int i = 0;
	for (i = 0; i < list_size(buffer); i++) {
		tr_datos* data = list_get(buffer, i);
		printf("\n%d - %s - %d - %d - %d - %s\n", data->nodo, data->ip,
				data->port, data->tamanio, data->bloque, data->tr_tmp);
	}
	printf(
			"\n------------------------------------------------------------------------\n");


	int sizeData = (sizeof(tr_datos)*list_size(buffer));
	void* sortedBuffer = malloc(sizeof(char)+sizeof(int)+sizeData);

	int* blocks = malloc(sizeof(int));
	*blocks = list_size(buffer);

	memcpy(sortedBuffer, "T", sizeof(char));
	memcpy(sortedBuffer+sizeof(char), blocks, sizeof(int));

	int index = 0;
	for(index=0; index < *blocks; index++){
		tr_datos* data = list_get(buffer, index);
		memcpy(sortedBuffer+sizeof(char)+sizeof(int)+(index*sizeof(tr_datos)), data, sizeof(tr_datos));
	}

	char opt;
	memcpy(&opt, sortedBuffer, sizeof(char));
	int blockes;
	memcpy(&blockes, sortedBuffer + sizeof(char), sizeof(int));

	printf("\n->La respuesta es: Op=%c, bloques=%d y tamanio=%d\n", opt,
			blockes, blockes * sizeof(tr_datos));

	int increment = 0;
	void* info = malloc(blockes*sizeof(tr_datos));
	memcpy(info, sortedBuffer+sizeof(char)+sizeof(int), blockes*sizeof(tr_datos));
	for (increment = 0; increment < blockes; increment++) {
		tr_datos* data = malloc(sizeof(tr_datos));
		memcpy(data, info + (increment * sizeof(tr_datos)), sizeof(tr_datos));
		printf("\nEn transformation.c-> %d - %s - %d - %d - %d - %s\n", data->nodo,
				data->ip, data->port, data->tamanio, data->bloque,
				data->tr_tmp);
	}

	return sortedBuffer;
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
