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

void addAvailBase(int avail);

void updateAvailability(char* algoritm, int avail);

void* replanTask(int master, int node, t_planningParams* params, t_job* job, int jobindex);

int getMaxWorkload(Yama* yama);

int hClockAvail(Yama* yama, elem_tabla_nodos* elem, int avail);

int setNewAvailability(elem_tabla_nodos* elem, char* algoritm, int avail);

int getHigerAvailNode(Yama* yama);

int blockExistInClock(block_info* blockRecived, int clock);

int increseClock(int clock);

tr_datos* buildNodePlaned(block_info* blockRecived, int master, int node_id);

tr_datos* updateNodeInTable(block_info* blockRecived, int master, int clock);

tr_datos* evaluateClock(block_info* blockRecived, int master, int clock,
		t_planningParams* planningParams);

tr_datos* doPlanning(block_info* blockRecived, int master,
		t_planningParams* planningParams);

#endif /* YAMAPLANNER_H_ */
