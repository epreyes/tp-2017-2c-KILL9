/*
 * connectionDataNodeFileSystem.c
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */
#include "headers/dataNode.h"
#include <protocol/protocol.h>
#include <connections/data_node_prot.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "headers/connectionDataNodeFileSystem.h"
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void connectionToFileSystem(){
	map_data_node();
	fileSystemSocket = connect_to_socket(data_node_config-> file_system_ip,data_node_config-> file_system_port);
	if(fileSystemSocket != 0){
		log_info(infoLogger,"Error al conectar al fileSystem.");
		perror("No se puede conectar al fileSystem");
		exit(EXIT_FAILURE);
	}

	PROTOCOL_DATANODE_TO_FILESYSTEM handshake = HANDSHAKE_CONNECTION_DATANODE_TO_FILESYSTEM;
	//Envio  mensaje al file system
	libtp_sendMessage(fileSystemSocket, &handshake,sizeof(PROTOCOL_DATANODE_TO_FILESYSTEM), infoLogger);

	//Espero respuesta del filesytem
	PROTOCOL_FILESYSTEM_TO_DATANODE handshakeResponse;
	int result =  libtp_receiveMessage(fileSystemSocket,&handshakeResponse,
			sizeof(PROTOCOL_FILESYSTEM_TO_DATANODE),infoLogger);

	//Manejo respuesta del FILESYTEM
	switch(RECEIVE_OK){
	case CLOSE_CONNECTION :
		log_info(infoLogger,"-> Se  desconecto el FILESYSTEM");
		printf("Se desconecto el FILESYSTEM");
		break;
	case RECEIVE_OK:
		if(handshakeResponse == HANDSHAKE_CONNECTION_FILESYSTEM_TO_DATANODE_OK){
				process_request_file_system(fileSystemSocket);
		}
		break;
	default:
		log_info(infoLogger,"-- Error al recibir los datos del FILESYSTEM");
		printf("-- Error al recibir los datos del FILESYSTEM ");
		break;

	}
}

void process_request_file_system(int  client_socket) {
	int ope_code = recv_operation_code(client_socket, infoLogger);
	while (ope_code != DISCONNECTED_CLIENT) {
		log_info(infoLogger, "CLIENT %d >> codigo de operacion : %d", client_socket, ope_code);
		switch (ope_code) {
			case GET_BLOQUE:
					get_block(client_socket);
			  break;
			default:
				  set_block(client_socket);
				break;
		}
		ope_code = recv_operation_code(client_socket, infoLogger);
	}
		close_client(client_socket);
		free(client_socket);
		return;
}
/*****************************************************
*	Implementacion para leer un bloque de un archivo   *
******************************************************/
void get_block(int client_socket){
		t_dn_get_block_req * request = dn_get_block_recv_req(client_socket,infoLogger);
		//TODO obtener el puntero al archivo memoria , leer y enviar las respues al cliente.	
		map_data_node();

		void * buffer = malloc(block_size);
		int pos = (request->number_block) * block_size;
}

void set_block(int  client_socket){


}

static void
check (int test, const char * message, ...)
{
    if (test) {
        va_list args;
        va_start (args, message);
        vfprintf (stderr, message, args);
        va_end (args);
        fprintf (stderr, "\n");
        exit (EXIT_FAILURE);
    }
}

int map_data_node() {
	/*El file descriptor */
	int fd;
	/*Informacion acerca del archivo */
	struct stat stat_data;
	int status;
	size_t size;

	//Nombre del archivo a abrir
	char * file_name = data_node_config->path_data_bin;

	/*Abrir el archivo para leer*/
	fd = open(file_name, O_RDONLY);
	check (fd < 0, "open %s failed: %s", file_name, strerror (errno));

	/*Obtener el tamanio del archivo */
	 status = fstat(fd,&stat_data);
	 check (status < 0, "stat %s failed: %s", file_name, strerror (errno));
	 size = stat_data.st_size;

	 /* Memory-map del archivo. */
	 mapped_data_node = mmap ((caddr_t) 0, size, PROT_READ, MAP_SHARED, fd, 0);
	 check (mapped_data_node == MAP_FAILED, "mmap %s failed: %s",file_name, strerror (errno));

	 /* Now do something with the information. */
//	int i;
//	for (i = 0; i < size; i++) {
//		char c;
//		c = ((char *) mapped_data_node)[i];
//		putchar(c);
//	}

	return EXIT_SUCCESS;
}


