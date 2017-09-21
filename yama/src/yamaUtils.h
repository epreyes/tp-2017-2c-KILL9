/*
 * yamaUtils.h
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#ifndef YAMAUTILS_H_
#define YAMAUTILS_H_

#include <commons/collections/list.h>
#include "yamaAdmin.h"

void* sortTransformationResponse(t_list* buffer, int size);

char* getTmpName(t_header* head, int blockId, int master);

bool compareTransformationBlocks(void* b1, void* b2);

void* buildPackage(void* masterRS, t_header head);

int sendPackage(Yama* yama, int master, void* package, int sizepack);

#endif /* YAMAUTILS_H_ */
