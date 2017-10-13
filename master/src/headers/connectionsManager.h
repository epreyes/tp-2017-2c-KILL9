/*
 * connectionsManager.h
 *
 *  Created on: 2/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_CONNECTIONSMANAGER_H_
#define HEADERS_CONNECTIONSMANAGER_H_

#include "master.h"
#include <arpa/inet.h>
#include <sys/socket.h>


void openYamaConnection(void);
int openNodeConnection(int node, char* direction);

#endif /* HEADERS_CONNECTIONSMANAGER_H_ */
