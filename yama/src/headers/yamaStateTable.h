/*
 * yamaStateTable.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_YAMASTATETABLE_H_
#define SRC_HEADERS_YAMASTATETABLE_H_

#include "yama.h"

int addToStatusTable(Yama* yama, int master, void* package);
void viewStateTable(Yama* yama);
void updateNodeList(void* fsInfo);

#endif /* SRC_HEADERS_YAMASTATETABLE_H_ */
