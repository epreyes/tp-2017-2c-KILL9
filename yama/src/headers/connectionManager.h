/*
 * connectionManager.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef CONNECTIONMANAGER_H_
#define CONNECTIONMANAGER_H_

#include <tplibraries/sockets/socket.h>

Client acceptMasterConnection(Server* server, fd_set* masterList, int hightSd);

int sendResponse(int master, void* masterRS);

int getMasterMessage(int socket, fd_set* mastersList);

int getSizeToSend(void* masterRS);

void exploreActivity(fd_set* mastersListTemp, fd_set* mastersList);

void waitMastersConnections();

#endif /* CONNECTIONMANAGER_H_ */
