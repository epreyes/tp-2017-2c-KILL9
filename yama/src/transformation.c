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

	char op2;
	int blocks;

	int sizeAdded = sizeof(char) + sizeof(int);

	if (op == 'R') {
		sizeAdded = sizeof(char) + sizeof(char) + sizeof(int);
		memcpy(&op2, response+sizeof(char), sizeof(char));
		memcpy(&blocks, response + sizeof(char) + sizeof(char), sizeof(int));
	}
	else{
		memcpy(&blocks, response + sizeof(char), sizeof(int));
	}

	int i = 0;
	for (i = 0; i < blocks; i++) {
		tr_datos* data = malloc(sizeof(tr_datos));
		memcpy(data,
				response + sizeAdded + (i * sizeof(tr_datos)),
				sizeof(tr_datos));
		printf("\n%d - %d - %s - %d\n", data->bloque, data->nodo, data->ip,
				data->port);
	}
}

void* processTransformation(int master, t_job* job) {
	log_trace(yama->log, "Atendiendo solicitud de Transformación. Job %d.",
			job->id);
	/*me traigo la informacion del archivo.*/
	elem_info_archivo* fsInfo = getFileInfo(master);

	if ((*(char*) fsInfo) == 'E') {
		return fsInfo;
	} else {
		/*Creo una lista, que va a ser la respuesta que se le va a mandar al master, sin planificar.*/
		t_list* nodeList = list_create();
		nodeList = buildTransformationResponseNodeList(fsInfo, master, job->id);

		return sortTransformationResponse(nodeList, master, fsInfo->filename,
				job);
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
		int master, int jobid) {

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
		setInStatusTable(jobid, 'T', master, nodeData->nodo,
				blockInfo->block_id, nodeData->tr_tmp, nodeData->bloque,
				fsInfo->filename);

		free(blockInfo);
	}

	return nodeList;
}

void* sortTransformationResponse(t_list* buffer, int master, char* fileName,
		t_job* job) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	int sizeData = (sizeof(tr_datos) * list_size(buffer));

	int sizeAdded = sizeof(char) + sizeof(int);
	if (job->replanificaciones > 0) {
		sizeAdded = sizeof(char) + sizeof(char) + sizeof(int);
	}

	void* sortedBuffer = malloc(sizeAdded + sizeData);

	int blocks = list_size(buffer);

	if (job->replanificaciones > 0) {
		memcpy(sortedBuffer, "R", sizeof(char));
		memcpy(sortedBuffer + sizeof(char), "T", sizeof(char));
		memcpy(sortedBuffer + sizeof(char) + sizeof(char), &blocks,
				sizeof(int));
	} else {
		memcpy(sortedBuffer, "T", sizeof(char));
		memcpy(sortedBuffer + sizeof(char), &blocks, sizeof(int));
	}

	int index = 0;
	for (index = 0; index < blocks; index++) {
		tr_datos* data = list_get(buffer, index);
		//creo el elemento para agregar a la tabla de planificados.
		addToTransformationPlanedTable(master, data, fileName);
		memcpy(
				sortedBuffer + sizeAdded + (index * sizeof(tr_datos)), data, sizeof(tr_datos));
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
