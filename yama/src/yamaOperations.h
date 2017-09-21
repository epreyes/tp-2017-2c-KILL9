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

void* processTransformation(t_header* head, void* fsInfo, int master);

int transformation(Yama* yama, t_header head, int master, void* fsInfo);

int localReduction(Yama* yama, t_header head, int master, void* fsInfo);

int globalReduction(Yama* yama, t_header head, int master, void* fsInfo);

int finalStore(Yama* yama, t_header head, int master, void* fsInfo);



#endif /* YAMAOPERATIONS_H_ */
