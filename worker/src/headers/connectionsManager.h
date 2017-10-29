/*
 * connectionsManager.h
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_CONNECTIONSMANAGER_H_
#define HEADERS_CONNECTIONSMANAGER_H_


#include "worker.h"

#include "transform.h"
#include "localReduction.h"
#include "globalReduction.h"
#include "finalStore.h"

void loadServer(void);
void readMasterBuffer(void);

#endif /* HEADERS_CONNECTIONSMANAGER_H_ */
