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
#include <tplibraries/sockets/socket.h>
#include <tplibraries/utils/utils.h>

#include "../protocol/protocol.h"

static char* CONFIG_PATH="./config/yamaConfig.properties";
static char* LOG_PATH="./log/yama.log";

typedef struct{

}rowStateTable;

typedef struct{
	char fs_ip[15];
	int fs_port;
	int fs_socket;
	int planning_delay;
	char balancign_algoritm[3];
	int port;
	Server yama_server;
	Client yama_client;
	t_log* log;
}Yama;


t_config* getConfig(Yama* yama);

void setProperties(Yama* yama, t_config* config);
/*-----------------------------*/
Yama configYama();

void waitForMasters();

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList, int hightSd);

int processRequest(Yama* yama, void* buff, int socket, int nbytes);

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp, fd_set* mastersList);

int getMasterMessage(Yama* yama, int socket, fd_set* mastersList);

int processRequest(Yama* yama, void* buff, int master, int nbytes);

/*
char* getTemporalFileName();
void getFileInfo();
*/

#endif /* YAMA_H_ */
