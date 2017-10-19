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

	viewStateTable();

	return sortTransformationResponse(nodeList);
}

void getTmpName(tr_datos* nodeData, int op, int blockId, int masterId) {
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp, op, masterId,
			blockId);

	strcpy(nodeData->tr_tmp, name);
}

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo,
		int master) {

	void* info = malloc(fsInfo->sizeInfo);
	memcpy(info, fsInfo->info, fsInfo->sizeInfo);

	t_list* nodeList = list_create();

	//Se calcularán los valores de disponibilidades para cada Worker
	updateAvailability();

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block_info* blockInfo = malloc(sizeof(block_info));
		memcpy(blockInfo, info + (index * sizeof(block_info)), sizeof(block_info));

		//Planifico
		tr_datos* nodeData = doPlanning(blockInfo, master);
		//agrego a la lista de respuesta
		list_add(nodeList, nodeData);

		//creo el elemento para agregar a la tabla de planificados.
		addToTransformationPlanedTable(master, nodeData);

		//actualizo la tabla de estados con la informacion del nuevo job.
		setInStatusTable('T', master, nodeData->nodo, getBlockId(nodeData->tr_tmp), nodeData->tr_tmp);

		free(blockInfo);
	}

	return nodeList;
}

void* sortTransformationResponse(t_list* buffer) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	int sizeData = (sizeof(tr_datos) * list_size(buffer));
	void* sortedBuffer = malloc(sizeof(char) + sizeof(int) + sizeData);

	int* blocks = malloc(sizeof(int));
	*blocks = list_size(buffer);

	memcpy(sortedBuffer, "T", sizeof(char));
	memcpy(sortedBuffer + sizeof(char), blocks, sizeof(int));

	int index = 0;
	for (index = 0; index < *blocks; index++) {
		tr_datos* data = list_get(buffer, index);
		memcpy(
				sortedBuffer + sizeof(char) + sizeof(int)
						+ (index * sizeof(tr_datos)), data, sizeof(tr_datos));
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
