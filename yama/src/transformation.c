/*
 * transformation.c
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#include "headers/transformation.h"

#include "headers/fileSystemConnector.h"

void viewTransformationResponse(void* response) {
	char op;
	memcpy(&op, response, sizeof(char));

	int blocks;
	memcpy(&blocks, response + sizeof(char), sizeof(int));

	int i = 0;
	for (i = 0; i < blocks; i++) {
		tr_datos* data = malloc(sizeof(tr_datos));
		memcpy(data,
				response + sizeof(char) + sizeof(int) + (i * sizeof(tr_datos)),
				sizeof(tr_datos));
		printf("\n%d - %d - %s - %d\n", data->bloque, data->nodo, data->ip,
				data->port);
	}
}

void* processTransformation(int master) {
	log_trace(yama->log, "Atendiendo solicitud de Transformación. Job %d.",
			yama->jobs + master);
	/*me traigo la informacion del archivo.*/
	elem_info_archivo* fsInfo = getFileInfo(master);

	if ((*(char*) fsInfo) == 'E') {
		return fsInfo;
	} else {
		/*Creo una lista, que va a ser la respuesta que se le va a mandar al master, sin planificar.*/
		t_list* nodeList = list_create();
		nodeList = buildTransformationResponseNodeList(fsInfo, master);

		return sortTransformationResponse(nodeList, master, 0);
	}
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

	t_planningParams* planningParams = getPlanningParams();

	void* info = malloc(fsInfo->sizeInfo);
	memcpy(info, fsInfo->info, fsInfo->sizeInfo);

	t_list* nodeList = list_create();

	//Se calcularán los valores de disponibilidades para cada Worker
	updateAvailability(planningParams->algoritm, planningParams->availBase);

	int index = 0;
	for (index = 0; index < fsInfo->blocks; index++) {
		block_info* blockInfo = malloc(sizeof(block_info));
		memcpy(blockInfo, info + (index * sizeof(block_info)),
				sizeof(block_info));

		//Planifico
		tr_datos* nodeData = doPlanning(blockInfo, master, planningParams);
		//agrego a la lista de respuesta
		list_add(nodeList, nodeData);

		//actualizo la tabla de estados con la informacion del nuevo job.
		setInStatusTable('T', master, nodeData->nodo, blockInfo->block_id,
				nodeData->tr_tmp, nodeData->bloque, fsInfo->filename);

		//creo el elemento para agregar a la tabla de planificados.
		addToTransformationPlanedTable(master, nodeData, fsInfo->filename);

		free(blockInfo);
	}

	return nodeList;
}

void* sortTransformationResponse(t_list* buffer, int master, int replaned) {
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

t_planningParams* getPlanningParams() {
	t_planningParams* params = malloc(sizeof(t_planningParams));

	params->availBase = yama->availBase;
	params->planningDelay = yama->planningDelay;
	strcpy(params->algoritm, yama->algoritm);

	return params;
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
