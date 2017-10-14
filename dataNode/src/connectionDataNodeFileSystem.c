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
int block_size = 1076;

void connectionToFileSystem(){

	map_data_node();
	fileSystemSocket = connect_to_socket(data_node_config-> file_system_ip,data_node_config-> file_system_port);

	int cod_ope =  DATANODE;

	int res = enviarMensaje(fileSystemSocket,&cod_ope,4,infoLogger);
	int cod_resp;

	res = recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);
	if(res < 1){
		puts("*****SE DESCONECTO EL FILESYSTEM");
		close(fileSystemSocket);
		free(data_node_config);
		config_destroy(data_node_config);
		exit(EXIT_FAILURE);
	}

	//Manejo respuesta del FILESYTEM
	switch(cod_resp){
		case ACCESOAPROBADO :
			printf("Se conecto al FILESYSTEM");
			process_request_file_system(fileSystemSocket);
			break;
		case ACCESODENEGADO :
			log_info(infoLogger,"-> Se  desconecto el FILESYSTEM");
			printf("No se conecto al FILESYSTEM");
			break;
		default:
			log_info(infoLogger,"-- Error al recibir los datos del FILESYSTEM");
			printf("-- Error al recibir los datos del FILESYSTEM ");
			break;
	}
}

void process_request_file_system(int client_socket) {
	int cod_resp;
	while (1) {
	   recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);

	   switch(cod_resp){
		case GET_BLOQUE:
			get_block(client_socket);
			break;
		default:
			set_block(client_socket);
			break;
	   }
	   break;
	}
	exit(EXIT_SUCCESS);
}
/*****************************************************
*	Implementacion para leer un bloque de un archivo   *
******************************************************/
void get_block(int client_socket){
	int cod_resp;

	map_data_node();
	void * buffer = malloc(block_size);
	// Recibo el numero de bloque
	recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);

	int num_block = cod_resp;
	void * pos = mapped_data_node + num_block * block_size;
	memcpy(buffer, pos, block_size);
	//Envio los datos del bloque

	/**tmp :solo para mostrar info*/
	printf("-- Informacion a enviar al FILESYSTEM ");
	int i;
	for (i = 0; i < block_size; i++) {
		char c;
		c = ((char *) buffer)[i];
		putchar(c);
	}
	printf("-- Fin Informacion a enviar al FILESYSTEM ");
	/*Fin : tmp */
	enviarMensaje(fileSystemSocket, buffer, block_size, infoLogger);
}

void set_block(int  client_socket){
	// Recibo el numero de bloque
	int num_block;
	recibirMensaje(client_socket, &num_block, 4, infoLogger);

	//Recibo los datos a escribir
	void * buffer = malloc(1076); //Reemplazar 1076 por el 1MB = 1048576
	recibirMensaje(client_socket,buffer,1076,infoLogger);

	void * pos = mapped_data_node + num_block * block_size;
	memcpy(pos, buffer, block_size);

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

	 /* Mostrar infoArchivo */
	 //	int i;
	 //	for (i = 0; i < size; i++) {
	 //		char c;
	 //		c = ((char *) mapped_data_node)[i];
	 //		putchar(c);
	 //	}
	 return EXIT_SUCCESS;
}

