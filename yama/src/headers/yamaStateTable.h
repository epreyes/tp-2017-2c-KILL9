/*
 * yamaStateTable.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_YAMASTATETABLE_H_
#define SRC_HEADERS_YAMASTATETABLE_H_

#include "yama.h"

int findNode(int node_id);

void viewNodeTable();

void viewPlannedTable();

void addToNodeList(void* fsInfo);

void updateNodeList(char op, int node_id);

void viewStateTable();

void viewLRPlannedTable();

int findInProcessTasks(int master, int node_id, int block, char op);

int getBlockId(char* tmpName);

void updateStatusTable(int master, char opCode, int node, int bloque, char status);

#endif /* SRC_HEADERS_YAMASTATETABLE_H_ */
