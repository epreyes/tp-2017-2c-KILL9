/*
 * localReduction.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_LOCALREDUCTION_H_
#define SRC_HEADERS_LOCALREDUCTION_H_

#include "yama.h"

void deleteOfPlanedList(int items, int master);

void viewLocalReductionResponse(void* response);

void* processLocalReduction(int master, int job);

void getLocalReductionTmpName(rl_datos* nodeData, int op, int blockId,
		int masterId);

int allTransformProcesFinish(int master, int jobid);

t_list* findTransformationPlaned(int master, int jobid);

#endif /* SRC_HEADERS_LOCALREDUCTION_H_ */
