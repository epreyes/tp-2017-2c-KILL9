/*
 * connectionDataNodeFileSystem.c
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */
#include "header/connectionDataNodeFileSystem.h"
#include "header/util.h"
#include <tplibraries/protocol/filesystem_datanode.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "header/dataNode.h"

int block_size = 1076;

void connectionToFileSystem(){
	log_info(infoLogger,"INICIANDO CONEXION CON FILESYSTEM ...\n");

	int cod_ope =  DATANODE;
	int res;
	int cod_resp;

	map_data_node();

	fileSystemSocket = connect_to_socket(data_node_config-> file_system_ip,data_node_config-> file_system_port);
	if( fileSystemSocket == -1){
		log_error(infoLogger,"Error al conectar con fileSystem\n");
	}

	res = enviarMensaje(fileSystemSocket,&cod_ope,4,infoLogger);
	if(res < 1){
		log_error(infoLogger,"Enviando Mensanje a fileSystem\n");
		exit(EXIT_FAILURE);
	}

	res = recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);
	if(res < 1){
		log_error(infoLogger,"Error al recibir mensaje fileSystem\n");
		exit(EXIT_FAILURE);
	}

	//Manejo respuesta del FILESYTEM
	switch(cod_resp){
		case ACCESOAPROBADO :
			log_info(infoLogger,"Accesso aprobado : se conecto al FILESYSTEM\n");
			process_request_file_system(fileSystemSocket);
			break;
		default:
			log_error(infoLogger,"Error al recibir codigo de respuesta del FILESYSTEM\n");
			break;
	}
	log_info(infoLogger,"FIN CONEXION CON FILESYSTEM ...\n");
}

void process_request_file_system(int client_socket) {
	log_info(infoLogger,"procesando request del fileSystem .. \n");

	int cod_op;
	while (1) {
	   recibirMensaje(fileSystemSocket, &cod_op, 4, infoLogger);

	   switch(cod_op){
		case GET_BLOQUE:
			log_info(infoLogger,"Codigo operacion GET_BLOQUE");
			get_block(client_socket);
			break;
		default:
			log_info(infoLogger,"Codigo operacion SET_BLOQUE");
			set_block(client_socket);
			break;
	   }
	  // break;
	}
	//exit(EXIT_SUCCESS);
	log_info(infoLogger,"Fin -> request del fileSystem");
}
/*****************************************************
*	Implementacion para leer un bloque de un archivo   *
******************************************************/
void get_block(int client_socket){
	log_info(infoLogger,"Iniciando lectura  bloque archivo ");
	int cod_resp;
	int num_block;

	void * buffer = malloc(block_size);
	// Recibo el numero de bloque
	recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);

	num_block = cod_resp;
	void * pos = mapped_data_node + num_block * block_size;
	memcpy(buffer, pos, block_size);

	/**tmp :solo para mostrar info*/
	printf("Informacion a enviar al FILESYSTEM \n");
	int i;
	for (i = 0; i < block_size; i++) {
		char c;
		c = ((char *) buffer)[i];
		putchar(c);
	}
	printf("Fin Informacion a enviar al FILESYSTEM \n");
	/*Fin : tmp */

	//Envio los datos del bloque
	log_info(infoLogger,"Envio los datos del bloque");
	enviarMensaje(fileSystemSocket, buffer, block_size, infoLogger);
	log_info(infoLogger,"Fin envio datos del bloque");

	log_info(infoLogger,"Fin lectura bloque archivo ");
}

/*****************************************************
*	Implementacion escritura de un bloque del archivo*
******************************************************/
void set_block(int  client_socket){
	log_info(infoLogger,"INICIO : escritura de un bloque del archivo ");

	// Recibo el numero de bloque
	int num_block;
	recibirMensaje(client_socket, &num_block, 4, infoLogger);

	//Recibo los datos a escribir
	void * buffer = malloc(1076); //Reemplazar 1076 por el 1MB = 1048576
	recibirMensaje(client_socket,buffer,1076,infoLogger);

	void * pos = mapped_data_node + num_block * block_size;
	memcpy(pos, buffer, block_size);

	log_info(infoLogger,"FIN  : escritura de un bloque del archivo ");
}

