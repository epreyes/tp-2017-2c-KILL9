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

/*Idem a libtp_sendMessage en esta caso se recibi los datos
 * Funcion que se encarga de recibir todos los bytes pedidos
 * Return -1 si no se pudo recibir
 * Return 0 si la conexion esta cerrada
 * Return 1  si se recibieron todos los datos *
 * En caso de producirse algun error loguea en un archivo
 * pasado por parametro
 */
int libtp_receiveMessage(int socket_fd,void* buffer,size_t bytesToRead,t_log *log){

	//recibo los datos
	int  receiveBytes = recv(socket_fd,buffer,bytesToRead,0);


	if(receiveBytes == -1){
		log_error(log,"Error al recibir los datos");
		perror("Error al recibir los datos");
		return -1;
	}else if(receiveBytes == 0){
		return 0 ;  //Close connection
	}else if (receiveBytes < bytesToRead ){
		char message =  malloc(300);
		log_error(log, message);
		log_error(log,"Se vuelve a llamar receiveMessage para enviar los "
				"restantes datos \n");
		free(message);
		//Se llama de nuevo a la funcion para recibir los restantes datos
		int  newBytes =  bytesToRead - receiveBytes;
		libtp_receiveMessage(socket_fd, buffer +  receiveBytes,newBytes,log);
	}else if( receiveBytes == bytesToRead){
		// Se recibieron todos los datos
		return 1;
	}
	return 1;
}

#endif /* CONNECTIONS_HANDLER_C_ */
