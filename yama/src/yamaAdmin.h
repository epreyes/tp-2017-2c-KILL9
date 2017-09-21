/*
 * yamaAdmin.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef YAMAADMIN_H_
#define YAMAADMIN_H_

#include "yama.h"
#include <commons/collections/list.h>

Yama configYama();

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList,
		int hightSd);

int getMasterMessage(Yama* yama, int socket, fd_set* mastersList);

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp,
		fd_set* mastersList);

void waitForMasters(Yama* yama);

#endif /* YAMAADMIN_H_ */
