/*
 * connectionDataNodeFileSystem.c
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */
#include "header/dataNode.h"
#include "header/connectionDataNodeFileSystem.h"
#include "header/util.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int block_size = 1000000;

void connectionToFileSystem() {
	log_info(infoLogger, "INICIANDO CONEXION CON FILESYSTEM ...\n");
	int blockQuantity;
	int res;
	int cod_resp;
	struct stat sb;

	map_data_node();

	fileSystemSocket = connect_to_socket(data_node_config->file_system_ip,data_node_config->file_system_port);
	if (fileSystemSocket == -1) {
		log_error(infoLogger, "Error al conectar con fileSystem\n");
	}

	if(stat(data_node_config ->path_data_bin,&sb) == -1){
		log_error(infoLogger,"Tamanio del archivo data.bin");
	}

	//cantidad de bloques
	//block_size : 1MB
	blockQuantity = sb.st_size / block_size;

	t_infoNodo infoNodo;
	infoNodo.handshake = NODE_HSK;
	infoNodo.block_quantity = blockQuantity;
	infoNodo.id_nodo = data_node_config->name_nodo;

	int dataSize = 0;
	char* workerIPPort = string_new();
	char*workerIP=data_node_config->worker_ip;
	char* workerPort= data_node_config->worker_port;

	dataSize += string_length(workerIP);
	dataSize += string_length(workerPort);
	dataSize += 1;
	string_append(&workerIPPort,workerIP);
	string_append(&workerIPPort,":");
	string_append(&workerIPPort,workerPort);

	infoNodo.data_size = dataSize;
	strcpy(infoNodo.worker_ip_port,workerIPPort);


	log_info(infoLogger,"Enviando informacion del nodo a FS .....\n");
	log_info(infoLogger,
			"-Handshake %d :  -Cantidad de bloque : %d -ID Nodo :%d \n -Tamaño data :%d -IP-Puerto FS : %d",
			infoNodo.handshake,
			infoNodo.block_quantity,
			infoNodo.id_nodo,
			infoNodo.data_size,
			workerIPPort);

	res = enviarMensaje(fileSystemSocket,&infoNodo,sizeof(int)*4,infoLogger);
	if(res < 1){
		log_error(infoLogger, "Envio IP worker fileSystem\n");
		exit(EXIT_FAILURE);
	}
	send(fileSystemSocket, infoNodo.worker_ip_port, dataSize,0);


	res = recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);
	if (res < 1) {
		log_error(infoLogger, "Respuesta FS a DataNode");
		exit(EXIT_FAILURE);
	}

	//Manejo respuesta del FILESYTEM
	switch (cod_resp) {
	case HSK_OK:
		log_info(infoLogger, "Accesso aprobado : se conecto al FILESYSTEM\n");
		process_request_file_system(fileSystemSocket);
		break;
	case ACCESODENEGADO:
		log_info(infoLogger,
				"Accesso denegado : no se puede conectar con el  FILESYSTEM\n");
		break;
	default:
		log_error(infoLogger,
				"Error al recibir codigo de respuesta del FILESYSTEM\n");
		break;
	}
	log_info(infoLogger, "FIN CONEXION CON FILESYSTEM ...\n");
}

void process_request_file_system(int client_socket) {
	log_info(infoLogger, "procesando request del fileSystem .. \n");
	t_leerBloque* t_bloque;
	int cod_op;
	while (1) {
		//recibirMensaje(fileSystemSocket, &cod_op, 4, infoLogger);
		t_bloque = recibirPaquete(client_socket);
		switch (t_bloque->idOperacion) {
		case GET_BLOQUE:
			log_info(infoLogger, "Codigo operacion GET_BLOQUE");
			get_block(client_socket, t_bloque);
			break;
		case SET_BLOQUE:
			log_info(infoLogger, "Codigo operacion GET_BLOQUE");
			set_block(client_socket, t_bloque);
			break;
		default:
			log_error(infoLogger, "Error con el codigo de operacion recibido");
			break;
		}
		// break;
	}
	//exit(EXIT_SUCCESS);
	log_info(infoLogger, "Fin -> request del fileSystem");
}
/*****************************************************
 *	Implementacion para leer un bloque de un archivo   *
 ******************************************************/
void get_block(int client_socket, t_leerBloque* t_bloque) {
	log_info(infoLogger, "Iniciando lectura  bloque archivo ");
	int cod_resp;
	int num_block;

	int block_cant = t_bloque->finByte;
	void * buffer = malloc(block_cant);
	// Recibo el numero de bloque
	//recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);

	//num_block = cod_resp;
	num_block = t_bloque->idBloque;

	void * pos = mapped_data_node + num_block * block_size;
	memcpy(buffer, pos, block_cant);

	/**tmp :solo para mostrar info*/
	printf("Informacion a enviar al FILESYSTEM \n");
	int i;
	for (i = 0; i < block_cant; i++) {
		char c;
		c = ((char *) buffer)[i];
		putchar(c);
	}
	printf("Fin Informacion a enviar al FILESYSTEM \n");
	/*Fin : tmp */

	//Envio los datos del bloque
	log_info(infoLogger, "Envio los datos del bloque");
	enviarMensaje(fileSystemSocket, buffer, block_cant, infoLogger);
	log_info(infoLogger, "Fin envio datos del bloque");

	log_info(infoLogger, "Fin lectura bloque archivo ");
}

/*****************************************************
 *	Implementacion escritura de un bloque del archivo*
 ******************************************************/
void set_block(int client_socket, t_leerBloque* t_bloque) {
	log_info(infoLogger, "INICIO : escritura de un bloque del archivo ");

	// Recibo el numero de bloque
	//int num_block;
	//recibirMensaje(client_socket, &num_block, 4, infoLogger);
	//Recibo los datos a escribir
	void * buffer = malloc(sizeof(char)*(t_bloque->finByte)); //Reemplazar 1076 por el 1MB = 1048576
	//recibirMensaje(client_socket,buffer,1076,infoLogger);

	void * pos = mapped_data_node + (t_bloque->idBloque) * block_size;
	memcpy(pos, t_bloque->contenido, t_bloque->finByte);

	int cod_ope = SET_BLOQUE_OK;

	int res = 0;
	res = enviarMensaje(client_socket, &cod_ope, 4, infoLogger);
	if (res < 1) {
		log_error(infoLogger, "error enviando Mensanje a fileSystem\n");
		exit(EXIT_FAILURE);
	}
	else
		log_info(infoLogger, "FIN  : escritura de un bloque del archivo ");
}

