#include "Socket.h"

/*------------------------------- Common functions -------------------------------------*/

/*
 *  openSocket: funcion commun para clientes y servidores. Devuelve el id del socket creado.*/
int openSocket() {
	return socket(AF_INET, SOCK_STREAM, 0);
}
/*--------------------------------------------------------------------------------------*/

/*------------------------------- CLIENT -----------------------------------------------*/

/*------------------------ private Client functions ------------------------------------*/

/*------------------------ public Client functions -------------------------------------*/
/* connectClient: realiza la conexion del cliente a la ip y puerto pasados como parametro.
 * Devuelve una estructura Client, con el socket_id ya conectado al server.*/
Client connectClient(char* ip, int port) {
	Client client;
	int sockfd;
	struct sockaddr_in dest_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);   // short, Ordenación de la red
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	memset(&(dest_addr.sin_zero), '\0', 8);

	client.address = dest_addr;
	client.socket_id = connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
	client.socket_server_id = sockfd;
	return client;
}

/* disconnectClient: cierra el socket cliente. */
int disconnectClient(Client* client){
 return close(client->socket_id);
}

/* getClientHostName: devuelve el nombre del host del cliente. Falta implementar. No se si es tan util. */
char* getClientHostName(Client* client) {
	//return server->host->h_name;
	return "localhost";
}

/* getClientIp: devuelve la ip del cliente. Falta implementar. No se si es tan util. */
char* getClientIp(Client* client) {
	//return inet_ntoa(*((struct in_addr *) server->host->h_addr));
	return "127.0.0.1";
}
/*--------------------------------------------------------------------------------------*/

/*------------------------------- SERVER -----------------------------------------------*/
/*------------------------ private Server functions ------------------------------------*/
int allowMultipleConnections(Server server) {
	int opt = TRUE;
	return setsockopt(server.server_socket, SOL_SOCKET, SO_REUSEADDR,
			(char *) &opt, sizeof(opt));
}

void createAddressStructure(Server* server, int port) {
	server->address.sin_family = AF_INET;
	server->address.sin_addr.s_addr = INADDR_ANY;
	server->address.sin_port = htons(port);
	char hostname[1024];
	gethostname(hostname, 1024);

	server->host = gethostbyname(hostname);
}

int bindSocket(Server* server) {
	return bind(server->server_socket, (struct sockaddr *) &(server->address),
			sizeof(server->address));
}

int startListen(Server* server) {
	return listen(server->server_socket, PENDING_CONECTIONS);
}

/*------------------------ public Server functions -------------------------------------*/
/* startServer: crea un servidor en la ip local y el puerto pasado como parametro.
 * Lo deja listo para recibir conexiones. Devuelve una estructura Server.*/
Server startServer(int port) {
	Server server;
	server.server_socket = openSocket();
	server.pending_connections = PENDING_CONECTIONS;
	allowMultipleConnections(server);
	createAddressStructure(&server, port);
	bindSocket(&server);
	server.higherSocketDesc = server.server_socket;
	server.status = startListen(&server);
	return server;
}

/* stopServer: cierra el socket en el cual se levanto el servidor */
int stopServer(Server* server) {
	return close(server->server_socket);
}

/* getServerHostName: devuelve el nombre del host en el cual se levanto el servidor. */
char* getServerHostName(Server* server) {
	return server->host->h_name;
}

/* getServerIp: devuelve la ip del host en el cual se levanto el servidor. */
char* getServerIp(Server* server) {
	return inet_ntoa(*((struct in_addr *) server->host->h_addr));
}
/*--------------------------------------------------------------------------------------*/
