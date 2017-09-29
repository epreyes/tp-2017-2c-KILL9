/*
 * yamaOperations.h
 *
 *  Created on: 14/09/2017
 *      Author: utnso
 */

#ifndef YAMAOPERATIONS_H_
#define YAMAOPERATIONS_H_

#include <commons/collections/list.h>
#include "yama.h"
#include <tplibraries/protocol/protocol.h>

/*-----------------------------------------------------------------------------------*/

void* processTransformation(Yama* yama, t_fileInfo* fsInfo, int master);

int transformation(Yama* yama, t_header head, int master, void* payload);

void* processLocalReduction(t_header* head, void* fsInfo, int master);

int localReduction(Yama* yama, t_header head, int master, void* fsInfo);

int globalReduction(Yama* yama, t_header head, int master, void* fsInfo);

int finalStore(Yama* yama, t_header head, int master, void* fsInfo);

char* getTmpName(int op, int blockId, int masterId);

bool compareTransformationBlocks(void* b1, void* b2);

t_list* buildTransformationResponseNodeList(Yama* yama, t_fileInfo* fsInfo,
		int master);

void* sortTransformationResponse(t_list* buffer);

tr_datos* buildMasterResponseBlock(int op, block* blockRecived, int master);

#endif /* YAMAOPERATIONS_H_ */
