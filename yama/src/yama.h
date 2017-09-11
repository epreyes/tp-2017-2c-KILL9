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
#include "../lib/socket/Socket.h"
#include "../protocol/protocol.h"

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
}Yama;


t_config* getConfig();
/*-----------------------------*/
Yama createYama();
void transformation();
void localReduction();
void globalReduction();
void finalStore();
char* getTemporalFileName();
void getFileInfo();


#endif /* YAMA_H_ */
