/*
 * fs_servidor.h
 *
 *  Created on: 15/10/2017
 *      Author: utnso
 */


#ifndef FS_SERVIDOR_H_
#define FS_SERVIDOR_H_

#include <arpa/inet.h>
#include <commons/log.h>
#include "estructuras.h" // esto debe ser deprecado
#include "fs.h"

#define MYPORT 5000    // Puerto al que conectarán los usuarios
#define BACKLOG 100     // Cuántas conexiones pendientes se mantienen en cola


void lanzarHiloServidor();
void *connection_handler_nodo(void *socket_desc);
void *connection_handler_yama(void *socket_desc);
void procesarPedidoYama(t_header pedido, int socket);
void procesarPedidoNodo(t_header pedido, int socket);


#endif /* FS_SERVIDOR_H_ */
