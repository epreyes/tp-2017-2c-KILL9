/*
 * yamaStateTable.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef YAMASTATETABLE_H_
#define YAMASTATETABLE_H_

#include "yama.h"

int addToStatusTable(Yama* yama, int master, void* package);
void viewStateTable(Yama* yama);
void updateNodeList(Yama* yama, void* fsInfo);

#endif /* YAMASTATETABLE_H_ */
