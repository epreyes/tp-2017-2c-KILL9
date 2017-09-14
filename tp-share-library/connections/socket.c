/*
 * socket.c

 *
 *  Created on: 12/9/2017
 *  Author: miguel tomicha
 *
 */

#include "../connections/socket.h"
#include <commons/config.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

int open_socket(int port,int countClient){

	struct sockaddr_in server;
	int fd_socket = socket(AF_INET,SOCK_STREAM,0);

	//Configuracion del servidor
	server.sin_family = AF_INET; //Familia TCP/IP
	server.sin_port = htons(port);//Puerto
	server.sin_addr.s_addr = INADDR_ANY; //Cualquier cliente puede conectarse

	if(bind(fd_socket,(struct sockaddr *)&server,sizeof(server) == -1)){
		perror("Error al abrir socket");
		close(fd_socket);
	}

	if (listen(fd_socket, countClient) == -1) {
		perror("Socket - error en el listen() ");
		close(fd_socket);
	};

	return fd_socket;
}

int close_socket(int fd_socket){
	return close(fd_socket);
}

int accept_connection(int fd_server_socket){
	struct sockaddr_in client;
	int length = sizeof(struct sockaddr_in);

	int fd_client_socket = accept(fd_server_socket,(struct sockaddr *)&client,(socklen_t *) &length);

	return fd_client_socket;
}
/**Guarda en socket_int el valor del fd del socket de conexion**/

int connect_to_servidor(char * server_ip,char * server_port,int * socket_int){
	struct addrinfo hints;
	struct addrinfo * server_info;

	memset(&hints,0,sizeof(hints));
	hints.ai_family= AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(getaddrinfo(server_ip,server_port,&hints,&server_info) !=0){
		perror("socket -> error getaddrinfo()\n");
		exit(1);
	}

	int fd_server_socket = socket(server_info->ai_family,server_info->ai_socktype,server_info->ai_socktype);

	if(fd_server_socket == -1){
		perror("socket -> error socket()\n");
		exit(1);
	}

	if(connect(fd_server_socket,server_info->ai_addr,server_info ->ai_addrlen)==-1){
		//libero los recursos
		close(fd_server_socket);
		perror("socket -> error en connect()\n");
		exit(1);
	}
	(*socket_int) = fd_server_socket;
	//libero recursos
	freeaddrinfo(server_info);

	return EXIT_SUCCESS;
}

int close_client(int fd_client_socket) {
	return close(fd_client_socket);
}





