/*
 * yamaStateTable.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "headers/tablesManager.h"

int existInStatusTable(int job, char op, int node) {
	if (!list_is_empty(yama->tabla_estados)) {
		int index = 0;
		for (index = 0; index < list_size(yama->tabla_estados); index++) {
			elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
			if (elem->job == job && elem->node == node && elem->op == op
					&& elem->status == 'P') {
				return 1;
			}
		}
		return 0;
	} else {
		return 0;
	}
}

void setInStatusTable(int job, char op, int master, int nodo, int bloque,
		char* tmpName, int nodeBlock, char* filename) {
	elem_tabla_estados* elemStatus = malloc(sizeof(elem_tabla_estados));
	elemStatus->block = bloque;
	elemStatus->job = job;
	elemStatus->master = master;
	elemStatus->node = nodo;
	elemStatus->node_block = nodeBlock;
	elemStatus->op = op;
	elemStatus->status = 'P';
	strcpy(elemStatus->tmp, tmpName);
	strcpy(elemStatus->fileProcess, filename);

	list_add(yama->tabla_estados, elemStatus);
}

int findNode(int node_id) {
	int index = 0;
	if (!list_is_empty(yama->tabla_nodos)) {
		for (index = 0; index < list_size(yama->tabla_nodos); index++) {
			elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
			if (node->node_id == node_id) {
				return index;
			}
		}
	}
	return -1;
}

void viewNodeTable() {
	printf("\n------------- Tabla de Nodos: %d -------------\n",
			list_size(yama->tabla_nodos));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
		printf(
				"ID: %d, Disponibilidad: %d, Tareas en Progreso: %d, Tareas Finalizadas: %d, errores: %d\n",
				node->node_id, node->availability, node->tasks_in_progress,
				node->tasts_done, node->errors);
	}
	printf("----------------------------------------------\n");
}

void viewPlannedTable() {
	printf(
			"\nLa cantidad de entradas de la tabla de tareas planificadas es: %d\n",
			list_size(yama->tabla_T_planificados));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_T_planificados); index++) {
		elem_tabla_planificados* planed = list_get(yama->tabla_T_planificados,
				index);
		printf("\nmaster: %d, node: %d, bloque: %d, temporal: %s\n",
				planed->master, planed->data->nodo, planed->data->bloque,
				planed->data->tr_tmp);
	}
}

void deleteOfPlanedListByNode(int items, int node, int master) {
	int i = 0;
	for (i = 0; i < items; i++) {
		int j = 0;
		for (j = 0; j < list_size(yama->tabla_T_planificados); j++) {
			elem_tabla_planificados* elem = list_get(yama->tabla_T_planificados,
					j);
			if (elem->data->nodo == node && elem->master == master) {
				list_remove(yama->tabla_T_planificados, j);
				break;
			}
		}

	}
}

void deleteItems(void* elem) {
	free(((elem_tabla_planificados*) elem)->data);
	free(elem);
}

void deleteFromPlanedTable(int master, int node) {
	int index = 0;
	t_list* itemsToRemove = list_create();
	for (index = 0; index < list_size(yama->tabla_T_planificados); index++) {
		elem_tabla_planificados* planed = list_get(yama->tabla_T_planificados,
				index);
		if (planed->master == master && planed->data->nodo == node) {
			list_add(itemsToRemove, planed);
		}
	}

	deleteOfPlanedListByNode(list_size(itemsToRemove), node, master);
	list_destroy_and_destroy_elements(itemsToRemove, &deleteItems);
}

void viewFileInfo() {
	printf(
			"\nLa cantidad de entradas de la tabla de informacion de archivos: %d\n",
			list_size(yama->tabla_info_archivos));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
		elem_info_archivo* info = list_get(yama->tabla_info_archivos, index);
		printf("\nBloques=%d - Filename=%s - Sizeinfo=%d\n", info->blocks,
				info->filename, info->sizeInfo);
	}
}

void viewLRPlannedTable() {
	printf(
			"\nLa cantidad de entradas de la tabla de reducciones locales es: %d\n",
			list_size(yama->tabla_LR_planificados));
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_LR_planificados); index++) {
		elem_tabla_LR_planificados* planed = list_get(
				yama->tabla_LR_planificados, index);
		printf(
				"\nmaster: %d, node: %d, ip: %s, port: %d, TRtemporal: %s, LRtemporal: %s\n",
				planed->master, planed->data->nodo, planed->data->ip,
				planed->data->port, planed->data->tr_tmp, planed->data->rl_tmp);
	}
}

void addToNodeList(void* fsInfo) {
	int* size = malloc(sizeof(int));
	memcpy(size, fsInfo, sizeof(int));

	void* nodes = malloc(*size);
	memcpy(nodes, fsInfo + sizeof(int), *size);

	int index = 0;
	for (index = 0; index < *size; index += sizeof(block_info)) {
		block_info* b = malloc(sizeof(block_info));
		memcpy(b, nodes + index, sizeof(block_info));
		int position = findNode(b->node1);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = config_get_int_value(yama->config,
					"NODE_AVAIL");
			node->considered = 0;
			node->node_id = b->node1;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			node->errors = 0;
			if (strcmp(b->node1_ip, "xxx") != 0) {
				list_add(yama->tabla_nodos, node);
			}
		}

		position = findNode(b->node2);
		if (position == -1) {
			elem_tabla_nodos* node = malloc(sizeof(elem_tabla_nodos));
			node->availability = config_get_int_value(yama->config,
					"NODE_AVAIL");
			node->considered = 0;
			node->node_id = b->node2;
			node->tasks_in_progress = 0;
			node->tasts_done = 0;
			node->errors = 0;
			if (strcmp(b->node2_ip, "xxx") != 0) {
				list_add(yama->tabla_nodos, node);
			}
		}
		free(b);
	}
	free(size);
	free(nodes);
}

void increaseNodeCharge(int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	node->availability--;
	node->tasks_in_progress++;
	if (node->errors > 0) {
		node->errors = 0;
	}
	list_replace(yama->tabla_nodos, index, node);
}

void deleteNodeErrors() {
	int index = 0;
	if (!list_is_empty(yama->tabla_nodos)) {
		for (index = 0; index < list_size(yama->tabla_nodos); index++) {
			elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
			node->errors = 0;
			list_replace(yama->tabla_nodos, index, node);
		}
	}
}

void increaseNodeError(int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	node->availability = 0;
	node->tasks_in_progress = 0;
	node->errors++;
	list_replace(yama->tabla_nodos, index, node);
}

void decreaseNodeCharge(int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	node->availability++;
	node->tasks_in_progress--;
	node->tasts_done++;
	list_replace(yama->tabla_nodos, index, node);
}

void updateNodeList(char op, int node_id) {
	int index = findNode(node_id);
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
	switch (op) {
	case 'T':
		node->availability++;
		node->tasks_in_progress--;
		node->tasts_done++;
		break;
	case 'L':
	case 'G':
		node->tasts_done++;
		break;
	case 'O':
		node->availability++;
		node->tasks_in_progress--;
		node->tasts_done++;
	}
}

char* getStageName(char op) {
	switch (op) {
	case 'T':
		return "Transformación";
		break;
	case 'L':
		return "Reducción Local";
		break;
	case 'G':
		return "Reducción Global";
		break;
	case 'S':
		return "Almacenamiento Final";
		break;
	}
	return "";
}

char* getStatusName(char status) {
	switch (status) {
	case 'P':
		return "En proceso";
		break;
	case 'F':
		return "Finalizado Ok";
		break;
	case 'E':
		return "Error";
		break;
	case 'A':
		return "Abortado";
		break;
	}
	return "";
}

void viewStateTable() {
	t_list* stateTable = yama->tabla_estados;
	printf("\n\n");
	printf(
			"------------------------------------------------------- Tabla de estados -------------------------------------------------------\n");
	printf("Cantidad de entradas: %d\n", list_size(stateTable));
	printf(
			"--------------------------------------------------------------------------------------------------------------------------------\n");
	int i = 0;
	while (i < list_size(stateTable)) {
		elem_tabla_estados* row = list_get(stateTable, i);
		i++;
		printf(
				"Job: %d - Master: %d - Nodo: %d - Bloque: %d - Etapa: %s - Archivo Temporal: %s - Estado: %s\n",
				row->job, row->master, row->node, row->block,
				getStageName(row->op), row->tmp, getStatusName(row->status));
		printf(
				"--------------------------------------------------------------------------------------------------------------------------------\n");
	}
}

int findInProcessTasks(int master, int node_id, int block, char op, t_job* job) {
	int index = 0;
	if (!list_is_empty(yama->tabla_estados)) {
		for (index = 0; index < list_size(yama->tabla_estados); index++) {
			elem_tabla_estados* node = list_get(yama->tabla_estados, index);

			if (op == 'T') {
				if ((node->job == job->id) && (node->node == node_id)
						&& (master == node->master)
						&& (block == node->node_block) && (op == node->op)
						&& (node->status == 'P')) {
					return index;
				}
			} else {
				if ((node->node == node_id) && (master == node->master)
						&& (op == node->op) && (node->status == 'P')
						&& (node->job == job->id)) {
					return index;
				}
			}

		}
	}
	return -1;
}

int getBlockId(char* tmpName) {
	char block_code[3];
	strncpy(block_code, tmpName + 24, 3);
	int code = 0;
	sscanf(block_code, "%d", &code);
	return code;
}

void updateStatusTable(int master, char opCode, int node, int bloque,
		char status, t_job* job) {
	int index = findInProcessTasks(master, node, bloque, opCode, job);
	elem_tabla_estados* row;
	if (index > -1) {
		row = list_get(yama->tabla_estados, index);
		row->op = opCode;
		row->status = status;
		list_replace(yama->tabla_estados, index, row);

	} else {
		printf(
				"\nNO PUEDE actualizar master %d, opCode %c, nodo %d, bloque %d, estado %c\n",
				master, opCode, node, bloque, status);
	}
}

void addToTransformationPlanedTable(int master, tr_datos* nodeData,
		char* fileName, int jobid) {
	elem_tabla_planificados* planed = malloc(sizeof(elem_tabla_planificados));
	planed->job = jobid;
	strcpy(planed->fileName, fileName);
	planed->data = malloc(sizeof(tr_datos));
	memcpy(planed->data, nodeData, sizeof(tr_datos));
	planed->master = master;
	list_add(yama->tabla_T_planificados, planed);
}

void addToLocalReductionPlanedTable(int master, rl_datos* nodeData,
		char* fileName, int jobid) {
	elem_tabla_LR_planificados* planed = malloc(
			sizeof(elem_tabla_LR_planificados));
	planed->job = jobid;
	planed->data = malloc(sizeof(rl_datos));
	memcpy(planed->data, nodeData, sizeof(rl_datos));
	planed->master = master;
	strcpy(planed->fileName, fileName);
	list_add(yama->tabla_LR_planificados, planed);
}

void addToGlobalReductionPlanedTable(int master, rg_datos* nodeData,
		char* fileName, int jobid) {
	elem_tabla_GR_planificados* planed = malloc(
			sizeof(elem_tabla_GR_planificados));
	planed->job = jobid;
	planed->data = malloc(sizeof(rg_datos));
	strcpy(planed->fileName, fileName);
	memcpy(planed->data, nodeData, sizeof(rg_datos));
	planed->master = master;
	list_add(yama->tabla_GR_planificados, planed);
}

void updateTasksAborted(int master, int node, char codeOp, t_job* job) {
	int index = 0;
	int sentError = 0;
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if ((elem->node == node) && (elem->master == master)
				&& (elem->op == codeOp) && (elem->status == 'P')
				&& (elem->job == job->id)) {
			updateStatusTable(master, codeOp, node, elem->node_block, 'E', job);
			if (sentError == 0) {
				sendErrorToFileSystem(elem->fileProcess);
				sentError = 1;
			}
		}
	}
}

t_list* getTaskFailed(int master, int node, t_job* job) {
	int index = 0;
	t_list* tasksFails = list_create();
	for (index = 0; index < list_size(yama->tabla_estados); index++) {
		elem_tabla_estados* elem = list_get(yama->tabla_estados, index);
		if ((elem->node == node) && (elem->master == master)
				&& (elem->op == 'T')
				&& (elem->job == job->id)) {
			list_add(tasksFails, elem);
			updateStatusTable(master, 'T', node, elem->node_block, 'E', job);
		}
	}
	return tasksFails;
}

block_info* findBlock(int block) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
		elem_info_archivo* elem = list_get(yama->tabla_info_archivos, index);
		void* info = malloc(elem->sizeInfo);
		memcpy(info, elem->info, elem->sizeInfo);
		int j = 0;
		for (j = 0; j < elem->blocks; j++) {
			block_info* blockInfo = malloc(sizeof(block_info));
			memcpy(blockInfo, info + (j * sizeof(block_info)),
					sizeof(block_info));
			if (blockInfo->block_id == block) {
				free(info);
				return blockInfo;
			}
			free(blockInfo);
		}
		free(info);
	}

	return NULL;
}
