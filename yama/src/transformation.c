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

	//viewStateTable();

	//viewNodeTable();

	//viewPlannedTable();

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

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block_info* blockInfo = malloc(sizeof(block_info));
		memcpy(blockInfo, info + (index * sizeof(block_info)),
				sizeof(block_info));

//planificar: me devuelve el nodeData que voy a meter en la lista.
//agregar nodo a la lista que se va a enviar, y al historial de planificacion.

		tr_datos* nodeData = malloc(sizeof(tr_datos));
		nodeData->nodo = blockInfo->node1;
		strcpy(nodeData->ip, blockInfo->node1_ip);
		nodeData->port = blockInfo->node1_port;
		nodeData->bloque = blockInfo->node1_block;
		nodeData->tamanio = blockInfo->end_block;
		getTmpName(nodeData, 'T', blockInfo->block_id, master);
		list_add(nodeList, nodeData);
		elem_tabla_planificados* planed = malloc(
				sizeof(elem_tabla_planificados));
		planed->data = malloc(sizeof(tr_datos));
		memcpy(planed->data, nodeData, sizeof(tr_datos));
		planed->master = master;
		list_add(yama->tabla_planificados, planed);
		setInStatusTable(nodeData->tr_tmp, nodeData->nodo, 'T', master);
		updateNodeList('T', nodeData->nodo);

		tr_datos* nodeData2 = malloc(sizeof(tr_datos));
		nodeData2->nodo = blockInfo->node2;
		strcpy(nodeData2->ip, blockInfo->node2_ip);
		nodeData2->port = blockInfo->node2_port;
		nodeData2->bloque = blockInfo->node2_block;
		nodeData2->tamanio = blockInfo->end_block;
		getTmpName(nodeData2, 'T', blockInfo->block_id, master);
		list_add(nodeList, nodeData2);
		elem_tabla_planificados* planed2 = malloc(sizeof(elem_tabla_planificados));
		planed2->data = malloc(sizeof(tr_datos));
		memcpy(planed2->data, nodeData2, sizeof(tr_datos));
		planed2->master = master;
		list_add(yama->tabla_planificados, planed2);
		setInStatusTable(nodeData2->tr_tmp, nodeData2->nodo, 'T', master);
		updateNodeList('T', nodeData2->nodo);

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
