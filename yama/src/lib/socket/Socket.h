/*
 * Socket.h

 *
 *  Created on: 3/9/2017
 *      Author: utnso
 *
 *
 */

/**
 * Como funciona?
 * Basicamente es un tad que evita tener que crear las estructuras para conectarse y hacer el bind, listen, etc,
 * ya que esos pasos se repiten siempre.
 * Para el caso de que un proceso sea un servidor, se crea una variable de tipo Server, y se inicializa usando
 * startServer(PUERTO).
 * Por ejemplo:
 * Server servidor = startServer(8080); Ahi ya queda encapsulado todo lo relacionado al socket, en la estructura
 * Server.
 * Despues para aceptar conexiones, hacer el select, enviar y recibir datos, se usan las funciones nativas.
 * Trate de hacer wrapers pero me hice lio con los punteros y lo deje. A parte es medio al pedo.
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
#define PENDING_CONECTIONS 10 //cantidad maxima de conexiones pendientes cuando se hace el accept()

/* Estructura para manejar servidores */
typedef struct{
	int server_socket; //id del socket del servidor.
	struct sockaddr_in address; //estructura de direccion.
	struct hostent* host; //informacion del host.
	int higherSocketDesc; //numero del mayor socket id. Se usa para el select.
	int pending_connections;
	int status;
}Server;

/* Estructura para manejar clientes */
typedef struct{
	int socket_id; //id del socket cliente.
	int socket_server_id; //id del socket al cual se esta conectado.
	struct sockaddr_in address; //estructura de direccion del cliente.
	struct hostent* host; //informacion del host.
}Client;


/*------------------------------- Common functions -------------------------------------*/
int openSocket();
/*--------------------------------------------------------------------------------------*/


/*------------------------------- CLIENT -----------------------------------------------*/
/*------------------------ private Client functions ------------------------------------*/


/*------------------------ public Client functions -------------------------------------*/

Client connectClient(char* ip, int port);

int disconnectClient(Client* client);

char* getClientHostName(Client* client);

char* getClientIp(Client* client);
/*--------------------------------------------------------------------------------------*/


/*------------------------------- SERVER -----------------------------------------------*/
/*------------------------ private Server functions ------------------------------------*/
int allowMultipleConnections(Server server);

void createAddressStructure(Server* server, int port);

int bindSocket(Server* server);

int startListen(Server* server);

/*------------------------ public Server functions -------------------------------------*/

Server startServer(int port);

int stopServer(Server* server);

char* getServerHostName(Server* server);

char* getServerIp(Server* server);
/*--------------------------------------------------------------------------------------*/


#endif /* SOCKET_H_ */
