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

void getTmpName(tr_datos* nodeData, int op, int blockId, int masterId);

void setInStatusTable(char op, int master, int nodo, int bloque, char* tmpName, int nodeBlock);

t_list* buildTransformationResponseNodeList(elem_info_archivo* fsInfo, int master);

void* sortTransformationResponse(t_list* buffer, int master);

bool compareTransformationBlocks(void* b1, void* b2);



#endif /* SRC_HEADERS_TRANSFORMATION_H_ */
