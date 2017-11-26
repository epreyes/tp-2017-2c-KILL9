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
		t_bloque = recibirPaquete(client_socket);
		if(t_bloque == NULL){
			log_error(infoLogger,"Error al recibir datos de FS");
			close(client_socket);
			return;

		}

		switch (t_bloque->idOperacion) {
		case GET_BLOQUE:
			log_info(infoLogger, "Codigo operacion GET_BLOQUE");
			get_block(client_socket, t_bloque);
			break;
		case SET_BLOQUE:
			log_info(infoLogger, "Codigo operacion SET_BLOQUE");
			set_block(client_socket, t_bloque);
			break;
		default:
			log_error(infoLogger, "Error con el codigo de operacion recibido");
			break;
		}
	}
	log_info(infoLogger, "Fin -> request del fileSystem");
}
/*****************************************************
 *	Implementacion para leer un bloque de un archivo   *
 ******************************************************/
void get_block(int client_socket, t_leerBloque* t_bloque) {
	log_info(infoLogger, "Iniciando lectura  bloque archivo %d \n",t_bloque ->idBloque);
	int cod_resp=GET_BLOQUE_OK;
	int num_block;
	int nroBloque=0;

	int block_cant = t_bloque->finByte;
	void * buffer = malloc(block_cant);
	// Recibo el numero de bloque
	//recibirMensaje(fileSystemSocket, &cod_resp, 4, infoLogger);
	recv(fileSystemSocket, &nroBloque, sizeof(int),0);

	//num_block = cod_resp;
	num_block = t_bloque->idBloque;

	void * pos = mapped_data_node + num_block * block_size;
	memcpy(buffer, pos, block_cant);

	//Envio los datos del bloque
	log_info(infoLogger, "Envio los datos del bloque");
	send(fileSystemSocket, &cod_resp, sizeof(int),0);
	send(fileSystemSocket, &block_cant, sizeof(int),0);
	send(fileSystemSocket, &nroBloque, sizeof(int),0);
//	enviarMensaje(fileSystemSocket, buffer, block_cant, infoLogger);
	send(fileSystemSocket, buffer, block_cant, MSG_WAITALL);
	log_info(infoLogger, "Fin envio datos del bloque");

	log_info(infoLogger, "Fin lectura  bloque archivo %d \n",t_bloque ->idBloque);
	free(buffer);
}

/*****************************************************
 *	Implementacion escritura de un bloque del archivo*
 ******************************************************/
void set_block(int client_socket, t_leerBloque* t_bloque) {
	log_info(infoLogger, "Iniciando escritura  bloque archivo %d \n",t_bloque ->idBloque);

	t_bloque->contenido=malloc(sizeof(char)* t_bloque->finByte);
    recv(client_socket, t_bloque->contenido,sizeof(char)* t_bloque->finByte, MSG_WAITALL);

	void * pos = mapped_data_node + (t_bloque->idBloque) * block_size;
	memcpy(pos, t_bloque->contenido, t_bloque->finByte);

	int cod_ope = SET_BLOQUE_OK;

	int res = 0;
	res = enviarMensaje(client_socket, &cod_ope, 4, infoLogger);
	if (res < 1) {
		log_error(infoLogger, "error enviando Mensanje a fileSystem\n");
		exit(EXIT_FAILURE);
	}
	else{
		log_info(infoLogger, "Fin escritura bloque archivo %d \n",t_bloque ->idBloque);
	}
	// Libero t_bloque
	free(t_bloque->contenido);
	free(t_bloque);

}

