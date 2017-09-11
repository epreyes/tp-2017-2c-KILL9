/*
 * Socket.h
 *
 *  Created on: 3/9/2017
 *      Author: utnso
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/select.h>

#define TRUE   1
#define FALSE  0
#define PORT 8888
#define PENDING_CONECTIONS 10
#define MAX_CLIENTS 30

typedef struct{
	int server_socket;
	struct sockaddr_in address;
	struct hostent* host;
	int clients_socket[MAX_CLIENTS];
	int higherSocketDesc;
	int pending_connections;
	int status;
}Server;

typedef struct{
	int socket_id;
	int sock_server_id;
	struct sockaddr_in address;
	struct hostent* host;
}Client;


/*------------------------------- Common functions -------------------------------------*/
int openSocket();
/*--------------------------------------------------------------------------------------*/


/*------------------------------- CLIENT -----------------------------------------------*/
/*------------------------ private Client functions ------------------------------------*/


/*------------------------ public Client functions -------------------------------------*/

int connectClient(Client* client, char* ip, int port);

//int stop(Client* server);

char* getClientHostName(Client* client);

char* getClientIp(Client* client);
/*--------------------------------------------------------------------------------------*/


/*------------------------------- SERVER -----------------------------------------------*/
/*------------------------ private Server functions ------------------------------------*/
int allowMultipleConnections(Server server);

void createAddressStructure(Server* server, int port);

int bindSocket(Server* server);

int startListen(Server* server);

void clearClientsList(Server* server);

/*------------------------ public Server functions -------------------------------------*/

Server startServer(int port);

int stop(Server* server);

char* getServerHostName(Server* server);

char* getServerIp(Server* server);
/*--------------------------------------------------------------------------------------*/


#endif /* SOCKET_H_ */
