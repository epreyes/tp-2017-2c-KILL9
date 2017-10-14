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

void* processTransformation(int master);

void* processTransformation(int master);

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo, int master);

void* sortTransformationResponse(t_list* buffer);

bool compareTransformationBlocks(void* b1, void* b2);


#endif /* SRC_HEADERS_TRANSFORMATION_H_ */
