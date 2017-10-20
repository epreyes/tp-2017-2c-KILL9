/*
 * connectionsManager.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"

void loadServer(void){

	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = INADDR_ANY;
	workerAddr.sin_port = htons(config_get_int_value(config,"WORKER_PUERTO"));
	int workerSocket= socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(workerSocket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(workerSocket, (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		log_error(logger,"No se pudo realizar el bind");
		exit(1);
	}

	log_info(logger,"Esperando conexiones de master");
	listen(workerSocket, 100);

	struct sockaddr_in masterAddr;
	unsigned int len;

	int masterSocket = accept(workerSocket, (void*) &masterAddr, &len);
	if(masterSocket!=-1){
		log_info(logger,"Conexión de Master %d recibida",masterSocket);
	}else{
		log_error(logger,"Error al establecer conexión");
		exit(1);
	}
	socket_worker=workerSocket;		//sacar
	socket_master=masterSocket;		//sacar
}
//==========================================================================

void readBuffer(){
		void* buffer = malloc(1);
		char operation;
		int bytesRecibidos = recv(socket_master, buffer, 1,MSG_WAITALL);
		if (bytesRecibidos <= 0) {
			log_error(logger,"el master %d se encuentra desconectado");
			exit(1);
		}

		memcpy(&operation,buffer,1);

		//hardcodeo datos
		char temporal[28];
		strcpy(temporal,"/tmp/1710131859-T-M000-B015");
		int position = 68;
		int size = 100000;

		//--------
		tr_node datos;

		switch(operation){
			case 'T':
				log_info(logger,"Solicitud de transformación recibida");
			//tamanio del archivo
				free(buffer);
				buffer = malloc(4);
				recv(socket_master, buffer, 4,0);
				memcpy(&(datos.fileSize),buffer,4);
				printf("\n\ntamanio:%d\n\n",datos.fileSize);
			//Archivo
				free(buffer);
				buffer = malloc(datos.fileSize);
				datos.file=malloc(datos.fileSize);
				recv(socket_master, buffer, datos.fileSize,0);
				strcpy(datos.file, buffer);
				printf("\n\nFile:%s\n\n",datos.file);
			//cant bloques
				free(buffer);
				buffer = malloc(sizeof(int));
				recv(socket_master, buffer, sizeof(int),0);
				memcpy(&(datos.blocksSize), buffer, sizeof(int));
				printf("\n\nFile:%d\n\n",datos.blocksSize);
			//bloques
				free(buffer);
				buffer = malloc(sizeof(block)*datos.blocksSize);
				datos.blocks=malloc(sizeof(block)*datos.blocksSize);
				recv(socket_master, buffer, sizeof(block)*datos.blocksSize,0);
				memcpy(datos.blocks, buffer,sizeof(block)*datos.blocksSize);
			//print_test
				int i;
				for (i = 0; i < (datos.blocksSize); ++i){
						printf("\t pos:%d  \t tam:%d \t tmp:%s\n", datos.blocks[i].pos, datos.blocks[i].size, datos.blocks[i].tmp);
				}

				//recorrer y generar un fork por cada bloque
				//cada transformBlock va a hacer el send a Master
				regenerateScript(datos.file,script_transform,"script.sh");
				for (i = 0; i < (datos.blocksSize); ++i){
					transformBlock(datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp);
				}

				free(datos.blocks);
				free(datos.file);
				break;
			case 'L':
				log_info(logger,"Solicitud de reducción local recibida");
				regenerateScript("hola mundo",script_reduction,"rd.sh");
				//reduceFile(file[],reductionScript);
				break;
			case 'G':
				log_info(logger,"Solicitud de reducción global recibida");
				//globalReduction(files[],reductionScript);
				break;
			case 'S':
				log_info(logger,"Solicitud de almacenadoFinal recibida");
				//finalStorage(???,???);
				break;
			default:
				log_error(logger,"No existe la operación solicita");
				close(socket_master);
				break;
				//cerrar conexion y devolver error
		}
		free(buffer);
	/*
	*/
}
