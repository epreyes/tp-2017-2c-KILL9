/*
 * finalStore.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_FINALSTORE_H_
#define SRC_HEADERS_FINALSTORE_H_

#include "yama.h"

void deleteOfGlobalReductionPlanedList(int items, int master);

void viewFinalStoreResponse(void* response);

void* processFinalStore(int master, int jobid);

int allGlobalReductionProcesFinish(int master);

t_list* findGlobalReductionPlaned(int master);

#endif /* SRC_HEADERS_FINALSTORE_H_ */
