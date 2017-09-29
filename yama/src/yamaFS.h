/*
 * yamaFS.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_YAMAFS_H_
#define SRC_YAMAFS_H_

#include "yamaStateTable.h"
#include <tplibraries/protocol/protocol.h>

void* getFileSystemInfo(Yama* yama, t_header head, void* payload);

int findFile(Yama* yama, char* fileName);

t_fileInfo* getFileInfo(Yama* yama, t_header head, char* fileName, int master);

#endif /* SRC_YAMAFS_H_ */
