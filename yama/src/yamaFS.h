/*
 * yamaFS.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_YAMAFS_H_
#define SRC_YAMAFS_H_

#include "yamaAdmin.h"
#include "yamaOperations.h"
#include <tplibraries/protocol/protocol.h>

int getFileSystemInfo(Yama* yama, t_header head, void* payload, int master);

#endif /* SRC_YAMAFS_H_ */
