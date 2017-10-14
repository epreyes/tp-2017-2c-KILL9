/*
 * socket.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef CONNECTIONS_SOCKET_H_
#define CONNECTIONS_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/string.h>
#include <commons/log.h>

typedef int socket_t;



/** Struct **/
typedef struct __attribute__((packed)) t_paquete{
	int idProceso;
	int cod_operacion;
	int tamanio;
	void* datos;
} t_paquete;


int connect_to_socket(char *server_ip, char *server_port);

socket_t open_socket(char* host, char*port) ;

socket_t accept_connection(socket_t listenSocket) ;

// Envia datos al Socket asignado

void enviarDatos(socket_t, int, int, int, void*);

// Recibir datos del Socket

t_paquete* recibirPaquete(socket_t);

int socket_recv(int  client_socket, void * buffer, int buffer_size) ;

int socket_send(int  server_socket, void * buffer, int buffer_size, int flags);

int socket_write(int  client_socket, void * response, int response_size) ;

int close_client(int);

int enviarMensaje(int sockfd, const void * datos, size_t bytesAenviar,t_log* t_log);

int recibirMensaje(int sockfd, void* buffer, size_t bytesAleer,t_log* log);


#endif /* CONNECTIONS_SOCKET_H_ */
