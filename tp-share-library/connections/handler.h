/*
 * handler.h
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#ifndef CONNECTIONS_HANDLER_H_
#define CONNECTIONS_HANDLER_H_
#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <errno.h>


#define LOG_LEVEL LOG_LEVEL_INFO
#define LOG_ERROR_PATH "error_logs"
#define LOG_INFO_PATH "info_logs"
#define LIBRARY_NAME "tpShareLibray"
#define MAX_CONNECTION_THREAD 20;
/**Manejo de respuesta de conexiones entre proceso **/
typedef enum RECEIVE_MESSAGE_RESULT{
	RECEIVE_ERROR    = -1,
	CLOSE_CONNECTION = 0,
	RECEIVE_OK		 = 1

}RECEIVE_MESSAGE_RESULT;

/*
 * Funcion encargada de  enviar TODOS los bytes requeridos
 * Return -1 si no se pudo enviar
 * Return -2 si hubo algun problema en la recursion
 * Return >0  si se enviaron todos los bytes (devuelve la cantidad)
 * Return 0 conexion cerrada
 * En caso de error guardar en el archivo de logs de la biblioteca
 */
int libtp_sendMessage(int, const void*, size_t, t_log*);

/*
 * Funcion que se encarga de recibir todos los bytes pedidos
 * Return -1 si no se pudo recibir
 * Return 0 si la conexion esta cerrada
 * Return 1  si se recibieron todos los datos *
 * En caso de producirse algun error loguea en un archivo
 * pasado por parametro
 */
int libtp_receiveMessage(int socke_fd,void* buffer,size_t byttesToRead,t_log *log);

#endif /* CONNECTIONS_HANDLER_H_ */
