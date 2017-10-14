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

void enviarDatos(socket_t fd, int idProceso, int cod_operacion, int tam,void* buff) {

	int res, tamanioPaquete = (3 * size_int) + tam;

	void* buffer = malloc(tamanioPaquete);
	//void* aux = buffer;
	memcpy(buffer, &idProceso, size_int);
	//aux += sizeof(int);
	memcpy(buffer+(size_int), &cod_operacion, size_int);
	//aux += sizeof(int);
	memcpy(buffer+(2*size_int), &tam, size_int);
	//aux += sizeof(int);
	memcpy(buffer+(3*size_int), buff, tam);
	//aux += tam;

	res = send(fd, buffer, tamanioPaquete, 0);

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


int socket_recv(int client_socket, void * buffer, int buffer_size) {
	return recv(client_socket, buffer, buffer_size, MSG_WAITALL);
}

int socket_send(int server_socket, void * buffer, int buffer_size, int flags) {
	return send( server_socket, buffer, buffer_size, MSG_WAITALL);
}

int close_client(int client_socket) {
	return close(client_socket);
}
int socket_write(int client_socket, void * response, int response_size) {
	return send(client_socket, response, response_size,MSG_WAITALL);
}

/*
 * Esta funcion se encarga de que se envien TODOS los bytes requeridos
 * Return -1 si no se pudo enviar
 * Return -2 si hubo algun problema en la recursion
 * Return >0  si se enviaron todos los bytes (devuelve la cantidad)
 * Return 0 conexion cerrada
 * En caso de error guardar en el archivo de logs de la biblioteca
 */
int enviarMensaje(int sockfd, const void * datos, size_t bytesAenviar,
		t_log* t_log) {

	int bytes_enviados = send(sockfd, datos, bytesAenviar, 0);

	if (bytes_enviados == -1) { //ERROR
		perror("send:");
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
		char* mensaje = malloc(200);
		sprintf(mensaje, "Se enviaron %d bytes de %d esperados", bytes_enviados,
				bytesAenviar);
		log_error(t_log, mensaje);
		log_error(t_log,
				"Entrando en recursion para enviar mensaje completo \n");
		free(mensaje);
		int nuevosBytes = bytesAenviar - bytes_enviados;
		enviarMensaje(sockfd, datos + bytes_enviados, nuevosBytes,
				t_log);
	} else if (bytes_enviados == bytesAenviar) {
		//mandado piola
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
int recibirMensaje(int sockfd, void* buffer, size_t bytesAleer,
		t_log* log) {

	int bytesHeader = recv(sockfd, buffer, bytesAleer, 0);

	//VALIDACIONES
	if (bytesHeader == -1) { //ERROR
		perror("recv:");
		log_error(log, "Error al recibir datos");
		return -1;
	} else if (bytesHeader == 0) { //SE CERRO LA CONEXION
		return 0;
	} else if (bytesHeader < bytesAleer) {
		char* mensaje = malloc(200);
		sprintf(mensaje,
				"Se recibieron %i bytes de %i esperados en la linea %d del archivo %s",
				bytesHeader, bytesAleer, __LINE__, __FILE__);
		log_error(log, mensaje);
		log_error(log, "Entrando en recursion para recibir mensaje completo\n");
		free(mensaje);
		int nuevosBytes = bytesAleer - bytesHeader;
		recibirMensaje(sockfd, buffer + bytesHeader, nuevosBytes,
				log);
	} else if (bytesHeader == bytesAleer) {
		//se envio ok!!
		return 1;
	}
	//FIN VALIDACIONES (loguea y retorna)

	return 1;
}















