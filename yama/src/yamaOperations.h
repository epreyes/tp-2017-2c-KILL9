/*
 * yamaOperations.h
 *
 *  Created on: 14/09/2017
 *      Author: amansilla
 */

#ifndef YAMAOPERATIONS_H_
#define YAMAOPERATIONS_H_

#include <commons/collections/list.h>
#include "yama.h"
#include "../protocol/protocol.h"

void loadResponse(block* response, void* buff);

int getFileSystemInfo(Yama* yama, header head, int master);

int processOperation(Yama* yama, header head, int master, void* buff);

int transformation(Yama* yama, header head, int master, void* buff);

int localReduction(Yama* yama, header head, int master, void* buff);

int globalReduction(Yama* yama, header head, int master, void* buff);

int finalStore(Yama* yama, header head, int master, void* buff);

#endif /* YAMAOPERATIONS_H_ */
