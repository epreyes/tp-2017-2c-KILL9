/*
 * socket.c

 *
 *  Created on: 12/9/2017
 *  Author: miguel tomicha
 *
 */

#include "socket.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

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

socket_t listen_Socket(char* host, char*port) {
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
	struct sockaddr_in hostRemoto;// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	size_t hostRemotoLen = sizeof(hostRemoto);
	socklen_t newSocket = accept(listenSocket,(struct sockaddr *) &hostRemoto, &hostRemotoLen);
	if (newSocket == -1){
		perror("Error al aceptar el socket");
	}
	return newSocket;
}

void enviarDatos(socket_t fd, int idProceso, int cod_operacion, int tam,
		void* buff) {

	int res, tamanioPaquete = (3 * sizeof(int)) + tam;

	void* buffer = malloc(tamanioPaquete);
	//void* aux = buffer;
	memcpy(buffer, &idProceso, sizeof(int));
	//aux += sizeof(int);
	memcpy(buffer+(sizeof(int)), &cod_operacion, sizeof(int));
	//aux += sizeof(int);
	memcpy(buffer+(2*sizeof(int)), &tam, sizeof(int));
	//aux += sizeof(int);
	memcpy(buffer+(3*sizeof(int)), buff, tam);
	//aux += tam;

	res = send(fd, buffer, tamanioPaquete, MSG_WAITALL);

	if (res == -1) {
		perror("No se pudo enviar los datos");
	}
	free(buffer);
}

t_paquete* recibirPaquete(socket_t fd) {

	t_paquete* paqueteRecivido = malloc(sizeof(t_paquete));
	int ret = 0;
	ret = recv(fd, &paqueteRecivido->idProceso, sizeof(int), MSG_WAITALL);

	if (ret == 0) {
		return NULL;
	}
	recv(fd, &paqueteRecivido->cod_operacion, sizeof(int), MSG_WAITALL);
	recv(fd, &paqueteRecivido->tamanio, sizeof(int), MSG_WAITALL);
	paqueteRecivido->datos = malloc(paqueteRecivido->tamanio);
	recv(fd, paqueteRecivido->datos, paqueteRecivido->tamanio, MSG_WAITALL);
	return paqueteRecivido;
}

int socket_recv(int * client_socket, void * buffer, int buffer_size) {
	return recv(* client_socket, buffer, buffer_size, MSG_WAITALL);
}

int socket_send(int * server_socket, void * buffer, int buffer_size, int flags) {
	return send(* server_socket, buffer, buffer_size, flags);
}

