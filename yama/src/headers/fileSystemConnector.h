/*
 * yamaFS.h
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_FILESYSTEMCONNECTOR_H_
#define SRC_HEADERS_FILESYSTEMCONNECTOR_H_

#include <tplibraries/protocol/filesystem_yama.h>
#include "tablesManager.h"

void* getFileSystemInfo(char* name);

int findFile(char* fileName);

elem_info_archivo* getFileInfo(int master);

#endif /* SRC_HEADERS_FILESYSTEMCONNECTOR_H_ */
