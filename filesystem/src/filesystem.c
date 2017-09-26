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

int main(void) {

	load_file_system_properties();

	loadServidorMultihread();

	return EXIT_SUCCESS;
}

void loadServidorMultihread(){
	t_paquete* paquete;
	int socket_fd;
	int  new_connection = open_socket("localhost",file_system_config->file_system_port);
	listen(new_connection,1024);

	while(1){
		socket_fd = accept_connection(new_connection);

		paquete = recibirPaquete(socket_fd);

		switch (paquete->idProceso) {
			case DATANODE:
				createThread(socket_fd);
				break;
			default	:
				break;
		}

	}
		close_socket(new_connection);
}

void createThread(int socket_fd){
	pthread_attr_t attr;
	pthread_t thread;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&thread, &attr, &process_request, (void *) &socket_fd);
	pthread_attr_destroy(&attr);
}

int process_request(void * socket_fd){
	int fd_datanode = *(int*) socket_fd;
	t_paquete* paquete;
	t_dn_get_block_resp * resp;

	log_info(infoLogger, "SE ACEPTO DATANODE A FD [%d] \n", fd_datanode);

	enviarDatos(fd_datanode, DATANODE, ACCESOAPROBADO, 2 * sizeof(int), &fd_datanode);
	resp = dn_get_block(fd_datanode,0,infoLogger);


	printf("-- Inicio Informacion recibida de dataNode");

	void * buffer = resp ->buffer;
	int i;
	for (i = 0; i < 1076; i++) {
		char c;
		c = ((char *) buffer)[i];
		putchar(c);
	}
	printf("-- Fin Informacion recibida de dataNode ");
}

//TODO mover a otro archivo de funciones
void load_file_system_properties(){
	config = config_create("./properties/fileSystem.properties");
	file_system_config = malloc(sizeof(t_file_system_config));
	file_system_config->file_sytem_ip= config_get_string_value(config,"IP_FILESYSTEM");
	file_system_config->file_system_port = config_get_string_value(config,"PORT_FILESYSTEM");
	free(config);

	/**creacion de log */
	infoLogger = log_create("fileSystem.log","FileSystem",0,LOG_LEVEL_INFO);
	errorLogger = log_create("fileSystemError.log","FileSystem",0,LOG_LEVEL_ERROR);
}
