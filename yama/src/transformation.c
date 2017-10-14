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

	return sortTransformationResponse(nodeList);
}

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo,
		int master) {

	void* info = malloc(fsInfo->sizeInfo);
	memcpy(info, fsInfo->info, fsInfo->sizeInfo);

	t_list* nodeList = list_create();

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block* blockInfo = malloc(sizeof(block));
		memcpy(blockInfo, info + (index * sizeof(block)), sizeof(block));

		tr_datos* nodeData = malloc(sizeof(tr_datos));
		nodeData->nodo = blockInfo->node1;
		strcpy(nodeData->ip, blockInfo->node1_ip);
		nodeData->port = blockInfo->node1_port;
		nodeData->bloque = blockInfo->node1_block;
		nodeData->tamanio = blockInfo->end_block;

		getTmpName(nodeData, 'T', nodeData->bloque, master);

		printf("\nTemporal====%s\n", nodeData->tr_tmp);

		nodeData->bloque_id = blockInfo->block_id;
		list_add(nodeList, nodeData);

		tr_datos* nodeData2 = malloc(sizeof(tr_datos));
		nodeData2->nodo = blockInfo->node2;
		strcpy(nodeData2->ip, blockInfo->node2_ip);
		nodeData2->port = blockInfo->node2_port;
		nodeData2->bloque = blockInfo->node2_block;
		nodeData2->tamanio = blockInfo->end_block;
		getTmpName(nodeData2, 'T', nodeData->bloque, master);
		nodeData2->bloque_id = blockInfo->block_id;
		list_add(nodeList, nodeData2);

	}

	return nodeList;
}

void* sortTransformationResponse(t_list* buffer) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	int size = sizeof(tr_datos) * list_size(buffer);
	char op = 'T';

	int buffIncrement=0;

	void* sortedBuffer = malloc(sizeof(char) + sizeof(int) + size);

	memcpy(sortedBuffer, &op, sizeof(char));
	buffIncrement += sizeof(char);

	memcpy(sortedBuffer + buffIncrement, &size, sizeof(int));

	int index = 0;
	for (index = 0; index < list_size(buffer); index++) {
		tr_datos* data = (tr_datos*) list_get(buffer, index);
		/*data->bloque
		data->bloque_id
		data->ip
		data->nodo
		data->port
		data->sizeTmpName
		data->tamanio
		data->tr_tmp*/

		printf("\n---- %s ---- %d - %d\n", data->tr_tmp, sizeof(*data), sizeof(tr_datos));

		memcpy(sortedBuffer+buffIncrement, &(data->bloque_id), sizeof(int));
		buffIncrement+=sizeof(int);
		memcpy(sortedBuffer+buffIncrement, &(data->nodo), sizeof(int));
		buffIncrement+=sizeof(int);
		memcpy(sortedBuffer+buffIncrement, &(data->ip), sizeof(char)*15);
		buffIncrement+=sizeof(char)*15;
		memcpy(sortedBuffer+buffIncrement, &(data->port), sizeof(int));
		buffIncrement+=sizeof(long);
		memcpy(sortedBuffer+buffIncrement, &(data->bloque), sizeof(int));
		buffIncrement+=sizeof(int);
		memcpy(sortedBuffer+buffIncrement, &(data->tr_tmp), sizeof(char)*31);
		buffIncrement+=sizeof(char)*31;

		/*memcpy(
				sortedBuffer
						+ ((sizeof(char) + sizeof(int))
								+ (index * sizeof(tr_datos))), data,
				sizeof(tr_datos));*/
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
