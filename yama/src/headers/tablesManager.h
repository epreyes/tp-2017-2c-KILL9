/*
 * yamaStateTable.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_TABLESMANAGER_H_
#define SRC_HEADERS_TABLESMANAGER_H_

#include "yama.h"


void viewNodeTable();

void viewPlannedTable();

void viewFileInfo();

void viewLRPlannedTable();

void addToNodeList(void* fsInfo);

void increaseNodeCharge(int node_id);

void decreaseNodeCharge(int node_id);

void updateNodeList(char op, int node_id);

void viewStateTable();

void addNewRowStatusTable(elem_tabla_estados* elem);

void updateStatusTable(int master, char opCode, int node, int bloque,
		char status);

void addToTransformationPlanedTable(int master, tr_datos* nodeData);

void addToLocalReductionPlanedTable(int master, rl_datos* nodeData);

void addToGlobalReductionPlanedTable(int master, rg_datos* nodeData);

void updateTasksAborted(int master, int node, int codeOp);

void setInStatusTable(char op, int master, int nodo, int bloque, char* tmpName,
		int nodeBlock);

int findInProcessTasks(int master, int node_id, int block, char op);

int getBlockId(char* tmpName);

int findNode(int node_id);

int existInStatusTable(int job, char op, int node);

t_list* getTaskFailed(int master, int node);

block_info* findBlock(int block);

#endif /* SRC_HEADERS_TABLESMANAGER_H_ */