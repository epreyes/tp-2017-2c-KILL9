/*
 * yama.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef YAMA_H_
#define YAMA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include "lib/socket/Socket.h"
#include "../protocol/protocol.h"

static char* CONFIG_PATH="./config/yamaConfig.properties";
static char* LOG_PATH="./log/yama.log";

typedef struct{

}rowStateTable;

typedef struct{
	char fs_ip[15];
	int fs_port;
	int planning_delay;
	char balancign_algoritm[3];
	int port;
	Server yama_server;
	Client yama_client;
	t_log* log;
}Yama;


t_config* getConfig();

void setProperties(Yama* yama, t_config* config);
/*-----------------------------*/
Yama configYama();

void waitForMasters();

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList, int hightSd);

int processRequest(Yama* yama, void* buff, int socket, int nbytes);

int getMasterMessage(Yama* yama, int socket, int* nbytes, fd_set* mastersList);

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp, fd_set* mastersList);

/*
void transformation();
void localReduction();
void globalReduction();
void finalStore();
char* getTemporalFileName();
void getFileInfo();
*/

#endif /* YAMA_H_ */
