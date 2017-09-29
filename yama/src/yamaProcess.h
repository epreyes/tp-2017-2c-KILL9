/*
 * yamaProcess.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_YAMAPROCESS_H_
#define SRC_YAMAPROCESS_H_

#include "yamaFS.h"
#include "yamaOperations.h"

void* processOperation(Yama* yama, int master, t_header head, void* payload);

void* processFileInfo(Yama* yama, int op, t_fileInfo* fsInfo, int master);


#endif /* SRC_YAMAPROCESS_H_ */
