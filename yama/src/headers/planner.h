/*
 * yamaPlanner.h
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#ifndef YAMAPLANNER_H_
#define YAMAPLANNER_H_

#include <commons/collections/list.h>
#include "yama.h"

void addAvailBase();

void updateAvailability();

void* replanTask(int master, int node);

int getMaxWorkload(Yama* yama);

int clockAvail(Yama* yama);

int hClockAvail(Yama* yama, elem_tabla_nodos* elem);

int setNewAvailability(elem_tabla_nodos* elem);

int setBaseAvailability();

int getHigerAvailNode(Yama* yama);

int blockExistInClock(block_info* blockRecived, int clock);

int increseClock(int clock);

tr_datos* buildNodePlaned(block_info* blockRecived, int master, int node_id);

tr_datos* updateNodeInTable(block_info* blockRecived, int master, int clock);

tr_datos* evaluateClock(block_info* blockRecived, int master, int clock);

tr_datos* doPlanning(block_info* blockRecived, int master);

#endif /* YAMAPLANNER_H_ */