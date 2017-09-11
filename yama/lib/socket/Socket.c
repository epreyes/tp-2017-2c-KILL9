#include "Socket.h"

/*------------------------------- Common functions -------------------------------------*/
int openSocket() {
	return socket(AF_INET, SOCK_STREAM, 0);
}
/*--------------------------------------------------------------------------------------*/

/*------------------------------- CLIENT -----------------------------------------------*/
/*------------------------ private Client functions ------------------------------------*/

/*------------------------ public Client functions -------------------------------------*/

int connectClient(Client* client, char* ip, int port) {
	int sockfd;
	struct sockaddr_in dest_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);   // short, Ordenación de la red
	dest_addr.sin_addr.s_addr = inet_addr(ip);
	memset(&(dest_addr.sin_zero), '\0', 8);

	client->address = dest_addr;
	client->socket_id = connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr));
	client->sock_server_id = sockfd;
	return client->socket_id;
}

/*int stop(Client* server){
 return 0;
 }*/

char* getClientHostName(Client* client) {
	//return server->host->h_name;
	return "localhost";
}

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

void clearClientsList(Server* server) {
	int i = 0;
	for (i = 0; i < MAX_CLIENTS; i++) {
		server->clients_socket[i] = 0;
	}
}

/*------------------------ public Server functions -------------------------------------*/
int start(Server* server, int port) {
	server->server_socket = openSocket();
	server->pending_connections = PENDING_CONECTIONS;
	allowMultipleConnections(*server);
	createAddressStructure(server, port);
	bindSocket(server);
	clearClientsList(server);
	server->higherSocketDesc = server->server_socket;
	return startListen(server);
}

int stop(Server* server) {
	return close(server->server_socket);
}

char* getServerHostName(Server* server) {
	return server->host->h_name;
}

char* getServerIp(Server* server) {
	return inet_ntoa(*((struct in_addr *) server->host->h_addr));
}
/*--------------------------------------------------------------------------------------*/
