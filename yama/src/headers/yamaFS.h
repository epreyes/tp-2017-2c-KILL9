/*
 * yamaFS.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_YAMAFS_H_
#define SRC_HEADERS_YAMAFS_H_

#include "yamaStateTable.h"
#include <tplibraries/protocol/protocol.h>

void* getFileSystemInfo(char* name);

int findFile(char* fileName);

elem_info_archivo* getFileInfo(int master);

#endif /* SRC_HEADERS_YAMAFS_H_ */
