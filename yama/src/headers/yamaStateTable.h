/*
 * yamaStateTable.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_YAMASTATETABLE_H_
#define SRC_HEADERS_YAMASTATETABLE_H_

#include "yama.h"

void updateStatusTable(elem_tabla_estados* elemToUpdate);
void viewStateTable();
void addToNodeList(void* fsInfo);
void viewNodeTable();
void updateNodeList(char op, int node_id);
int getBlockId(char* tmpName);
void viewPlannedTable();

#endif /* SRC_HEADERS_YAMASTATETABLE_H_ */
