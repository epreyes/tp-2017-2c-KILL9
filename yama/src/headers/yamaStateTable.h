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

int findRow(int master, int node_id, int block, char op);

int getBlockId(char* tmpName);

void updateStatusTable(elem_tabla_estados* elem);

#endif /* SRC_HEADERS_YAMASTATETABLE_H_ */
