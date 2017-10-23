#include "socket.h"

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
 return close(client->socket_server_id);
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



int size_int = sizeof(int);

struct addrinfo* configurarAddrinfo(char* host, char*port) {
	struct addrinfo addrinf, *serverInfo;
	int16_t res;
	memset(&addrinf, 0, sizeof(addrinf));
	addrinf.ai_family = AF_UNSPEC;
	addrinf.ai_socktype = SOCK_STREAM;
	if (!strcmp(host, "localhost")) {
		addrinf.ai_flags = AI_PASSIVE;
		res = getaddrinfo(NULL, port, &addrinf, &serverInfo);
	} else
		res = getaddrinfo(host, port, &addrinf, &serverInfo);
	if (res) {
		error_show("Error al hacer getaddrinfo() -> %s \n", gai_strerror(res));
		exit(EXIT_FAILURE);
	}
	return serverInfo;
}

/** conexion a socket*/

int connect_to_socket(char *host,char * port){

	struct addrinfo* serverInf = configurarAddrinfo(host, port);
	if (serverInf == NULL) {
		exit(EXIT_FAILURE);
	}
	int serverSock = socket(serverInf->ai_family, serverInf->ai_socktype,
			serverInf->ai_protocol);

	if (serverSock == -1) {
		error_show("No se pudo conectar \n", errno);
		exit(EXIT_FAILURE);
	}
	int connect_result =connect(serverSock, serverInf->ai_addr, serverInf->ai_addrlen);
	if (connect_result == -1) {
		error_show("No se pudo conectar con el server \n", errno);
		close(serverSock);
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(serverInf);
	return serverSock;
}

socket_t open_socket(char* host, char*port) {
	struct addrinfo* serverInf = configurarAddrinfo(host, port);
	if (serverInf == NULL) {
		exit(EXIT_FAILURE);
	}

	int socketEscuchar;
	socketEscuchar = socket(serverInf->ai_family, serverInf->ai_socktype,
			serverInf->ai_protocol);
	int activo = 1;
	setsockopt(socketEscuchar, SOL_SOCKET, SO_REUSEADDR, &activo, sizeof(int));
	bind(socketEscuchar, serverInf->ai_addr, serverInf->ai_addrlen);

	freeaddrinfo(serverInf);
	return socketEscuchar;
}

socket_t accept_connection(socket_t listenSocket) {
	struct sockaddr_in client;// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	size_t clientLength = sizeof(struct sockaddr_in);
	socklen_t newSocket = accept(listenSocket,(struct sockaddr *) &client,(socklen_t *)&clientLength);
	if (newSocket == -1){
		perror("Error al aceptar el socket");
	}
	return newSocket;
}



/*
 * Esta funcion se encarga de que se envien TODOS los bytes requeridos
 * Return -1 si no se pudo enviar
 * Return >0  si se enviaron todos los bytes (devuelve la cantidad)
 * Return 0 conexion cerrada
 * En caso de error guardar en el archivo de logs de la biblioteca
 */
int enviarMensaje(int sockfd, const void * datos, size_t bytesAenviar,
		t_log* t_log) {

	int bytes_enviados = send(sockfd, datos, bytesAenviar, 0);

	if (bytes_enviados == -1) { //ERROR
		log_info(t_log, "No se pudieron enviar datos");
		return -1;
	} else if (bytes_enviados == 0) { //conexion cerrada
		char* mensaje = string_new();
		string_append(&mensaje, "Se desconecto el cliente con el socket  ");
		string_append(&mensaje, sockfd);
		log_info(t_log, mensaje);
		free(mensaje);
		return 0;
	} else if (bytes_enviados < bytesAenviar) {
		log_error(t_log,"Entrando en recursion para enviar mensaje completo \n");
		int nuevosBytes = bytesAenviar - bytes_enviados;
		enviarMensaje(sockfd, datos + bytes_enviados, nuevosBytes,t_log);
	} else if (bytes_enviados == bytesAenviar) {
		return bytes_enviados;
	}

	return bytes_enviados;
}

/*
 * Funcion que se asegura de recibir todos los bytes pedidos
 * Return -1 si no se pudo recibir
 * Return 0 si la conexion esta cerrada
 * Return 1  si se recibieron todos los datos *
 * En caso de errores loguea en un archivo
 */
int recibirMensaje(int sockfd, void* buffer, size_t bytesAleer,t_log* log) {

	int bytesHeader = recv(sockfd, buffer, bytesAleer, 0);

	//VALIDACIONES
	if (bytesHeader == -1) {
		log_error(log, "Error al recibir datos");
		return -1;
	} else if (bytesHeader == 0) { //SE CERRO LA CONEXION
		return 0;
	} else if (bytesHeader < bytesAleer) {
		log_error(log, "Entrando en recursion para recibir mensaje completo\n");
		int nuevosBytes = bytesAleer - bytesHeader;
		recibirMensaje(sockfd, buffer + bytesHeader, nuevosBytes,log);
	} else if (bytesHeader == bytesAleer) {
		//se envio ok!!
		return 1;
	}
	return 1;
}

