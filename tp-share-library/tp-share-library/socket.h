/*
 * socket.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef TP_SHARE_LIBRARY_SOCKET_H_
#define TP_SHARE_LIBRARY_SOCKET_H_

#include "../tp-share-library/socket.h"

#include <commons/config.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>

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
 *
 * @RETURN int : file descriptor del socket servidor
 */
int connect_to_socket(char *, char *);

/**
 * @NAME:  close_client
 * @DESC:  Cierra la conexion de un cliente
 *
 * @PARAMS int * client_socket : file descriptor del socket cliente a cerrar
 *
 * @RETURN int : 0 funcionamiento normal, -1 en caso de error
 */
int close_client(int);


#endif /* TP_SHARE_LIBRARY_SOCKET_H_ */
