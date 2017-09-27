/*
 * yamaProcess.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_YAMAPROCESS_H_
#define SRC_YAMAPROCESS_H_

#include "yamaAdmin.h"
#include "yamaFS.h"
#include "yamaOperations.h"

int processRequest(Yama* yama, void* request, int master, int nbytes);

int processOperation(Yama* yama, int master, t_header head, void* fsInfo);

#endif /* SRC_YAMAPROCESS_H_ */
