/*
 * handler.c
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#ifndef CONNECTIONS_HANDLER_C_
#define CONNECTIONS_HANDLER_C_

#include "handler.h"


/*
 * Funcion para enviar TODOS los bytes requeridos
 * Return -1 si no se pudo enviar
 * Return -2 si hubo algun problema en la recursion
 * Return >0  si se enviaron todos los bytes (devuelve la cantidad)
 * Return 0 conexion cerrada
 * En caso de error guardar en el archivo de logs de la biblioteca
 */
int libtp_sendMessage(int socket_fd, const void * data, size_t bytesToSend,t_log* t_log){

	int send_bytes = send(socket_fd,data,bytesToSend,0);

	if(send_bytes == -1){//error la enviar datos
		perror("Send():");
		log_info(t_log,"Error al enviar los datos");
		return -1;
	}else if(send_bytes == 0 ){ //cerrada la conexion
		char *message = string_new();
		string_append(&message,"Se desconecto el client con el socket :");
		string_append(&message,socket_fd);
		log_info(t_log,message);
		//libero recursos
		free(message);
		return 0;
	} else if (send_bytes < bytesToSend) {
			char* message= malloc(300);
			sprintf(message, "Se enviaron %d bytes de %d esperados", send_bytes,
					bytesToSend);
			log_error(t_log, message);
			log_error(t_log,
					"Enviado el resto de los bytes para enviar mensaje completo \n");
			free(message);
			int newBytes = bytesToSend - send_bytes;
			libtp_sendMessage(socket_fd, data + send_bytes, newBytes,
					t_log);
		} else if (send_bytes == bytesToSend) {
			//Envio toda la info
			return send_bytes;
		}

		return send_bytes;
}

#endif /* CONNECTIONS_HANDLER_C_ */
