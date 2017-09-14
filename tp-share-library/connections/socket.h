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

/**
 * Crear un socket servidor
 * Abre un puerto y espera que se conecten clientes
 *
 */
int open_socket(int port ,int countClient);

/**
 *file descriptor a cerrar
 */
int close_socket(int fd_socket);


/**
 * @NAME:  connect_to_socket
 * @DESC:  Acepta la conexion de un cliente al socket
 *
 * @PARAMS int fd_listening_socket :fd del socket que va a aceptar conexiones
 *
 * @RETURN int : fd del del cliente que se conecto
 */
int accept_connection(int);

/**
 * @NAME:  connect_to_socket
 * @DESC:   conectarse a un socket servidor
 *
 * @PARAMS char * server_ip   : ip del socket servidor
 * 		   char * server_port : puerto del socket servidor
 *		   int * server_int  : El valor del fd del socket de conexion
 * @RETURN int : 0 en caso de exito
 */
int connect_to_servidor(char *server_ip, char *server_port,int *fdsocket);

/**
 * @NAME:  close_client
 * @DESC:  Cierra la conexion de un cliente
 *
 * @PARAMS int * client_socket : file descriptor del socket cliente a cerrar
 *
 * @RETURN int : 0 funcionamiento normal, -1 en caso de error
 */
int close_client(int);


#endif /* CONNECTIONS_SOCKET_H_ */
