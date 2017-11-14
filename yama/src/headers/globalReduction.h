/*
 * globalReduction.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_GLOBALREDUCTION_H_
#define SRC_HEADERS_GLOBALREDUCTION_H_

#include "yama.h"

void viewGlobalReductionResponse(void* response);

void deleteOfLocalReductionPlanedList(int items, int master);

void getGlobalReductionTmpName(rg_datos* nodeData, int op, int blockId,
		int masterId);

void* processGlobalReduction(int master, int jobid);

int allLocalReductionProcesFinish(int master);

t_list* findLocalReductionPlaned(int master);

int getLastChargedNode(t_list* planed);

#endif /* SRC_HEADERS_GLOBALREDUCTION_H_ */
