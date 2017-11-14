/*
 * transformation.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_TRANSFORMATION_H_
#define SRC_HEADERS_TRANSFORMATION_H_

#include "yama.h"
#include <commons/collections/list.h>

void viewTransformationResponse(void* response);

void* processTransformation(int master, int jobid);

void getTmpName(tr_datos* nodeData, int op, int blockId, int masterId);

void* sortTransformationResponse(t_list* buffer, int master, char* fsInfo);

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo,
		int master, int jobid);

bool compareTransformationBlocks(void* b1, void* b2);

t_planningParams* getPlanningParams();

#endif /* SRC_HEADERS_TRANSFORMATION_H_ */
