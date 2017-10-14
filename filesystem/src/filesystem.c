/*
 ============================================================================
 Name        : filesystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <commons/config.h>
#include <connections/socket.h>
#include <connections/data_node_prot.h>
#include <protocol/protocol.h>
#include <pthread.h>
#include <stdlib.h>
#include <struct/struct.h>

#include "header/fileSystem.h"

/* Declaracion del mutex*/
pthread_mutex_t mutexSock;

int main(void) {

	load_file_system_properties();

	//loadServidorMultihread();
	connectionFileSystemDataNode();
	return EXIT_SUCCESS;
}
/*
 *
void loadServidorMultihread(){
	t_paquete* paquete;
	int socket_fd;
	int  new_connection = open_socket("localhost",file_system_config->file_system_port);
	listen(new_connection,1024);
	int cod_ope ;
	while(1){
		/*Inicializacion del mutex */
		//pthread_mutex_init(&mutexSock,NULL);
		//pthread_mutex_lock(&mutexSock);
		//socket_fd = accept_connection(new_connection);
		//log_info(infoLogger, "SE ACEPTO DATANODE A FD [%d] \n", socket_fd);
		//recibirMensaje(socket_fd, &cod_ope, 4, infoLogger);
		//log_info(infoLogger, "DESPUES DE RECIBIR MSJ A FD [%d] \n", socket_fd);
		//paquete = recibirPaquete(socket_fd);

		//switch (paquete->idProceso) {
		//switch (cod_ope) {
		//	case DATANODE:
		//		createThread(socket_fd);
		//		break;
		//	default	:
		//		break;
		//}

	//}
	//	close_socket(new_connection);
//}

/**
void createThread(int socket_fd){
	pthread_attr_t attr;
	pthread_t thread;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, &process_request, (void *) &socket_fd);
	pthread_attr_destroy(&attr);
}
*/
void connectionFileSystemDataNode(){
	int socket_fd;
	int cod_ope ;

	int  new_connection = open_socket("localhost",file_system_config->file_system_port);
	listen(new_connection,1024);

	socket_fd = accept_connection(new_connection);
	log_info(infoLogger, "SE ACEPTO DATANODE A FD [%d] \n", socket_fd);
	recibirMensaje(socket_fd, &cod_ope, 4, infoLogger);
	log_info(infoLogger, "DESPUES DE RECIBIR MSJ A FD [%d] \n", socket_fd);


	int code_ope = 3;
	int res=  enviarMensaje(socket_fd, &code_ope,4,infoLogger);

	int code_bloque = GET_BLOQUE;
	res =  enviarMensaje(socket_fd, &code_bloque,4,infoLogger);


	int num_bloque = 0;
	res =  enviarMensaje(socket_fd, &num_bloque, 4, infoLogger);
	void * buffer = malloc(1076);

	recibirMensaje(socket_fd,buffer,1076,infoLogger);


	printf("-- Inicio Informacion recibida de dataNode");

	//void * buffer = paquete->datos;
	int i;
	for (i = 0; i < 1076; i++) {
		char c;
		c = ((char *) buffer)[i];
		putchar(c);
	}
	printf("-- Fin Informacion recibida de dataNode ");
	return ;
}

//TODO mover a otro archivo de funciones
void load_file_system_properties(){
	config = config_create("./properties/fileSystem.properties");
	file_system_config = malloc(sizeof(t_file_system_config));
	file_system_config->file_system_ip= config_get_string_value(config,"IP_FILESYSTEM");
	file_system_config->file_system_port = config_get_string_value(config,"PORT_FILESYSTEM");
	free(config);

	/**creacion de log */
	infoLogger = log_create("fileSystem.log","FileSystem",true,LOG_LEVEL_INFO);
	errorLogger = log_create("fileSystemError.log","FileSystem",true,LOG_LEVEL_ERROR);
}
