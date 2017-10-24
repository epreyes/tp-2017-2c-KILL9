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
	log_trace(logger, "Worker iniciado en puerto %d", config_get_int_value(config,"WORKER_PUERTO"));
	log_info(logger,"Esperando conexiones de master");
	listen(workerSocket, 100);

	struct sockaddr_in masterAddr;
	unsigned int len;

	int masterSocket = accept(workerSocket, (void*) &masterAddr, &len);
	if(masterSocket!=-1){
		log_trace(logger,"Master %d: Conectado",masterSocket);
	}else{
		log_error(logger,"Error al establecer conexión con Master");
		exit(1);
	}
	socket_worker=workerSocket;
	socket_master=masterSocket;
}
//==========================================================================

void readSocketBuffer(int socket,int size,void* destiny){
	void* buffer = malloc(size);
	int bytesReaded = recv(socket, buffer, size, MSG_WAITALL);
	if (bytesReaded <= 0) {
		log_warning(logger,"Master %d: desconectado",socket);
		exit(1);
	}
	memcpy(destiny, buffer, size);
	free(buffer);
}

void readBuffer(){
	char operation;
	readSocketBuffer(socket_master,sizeof(char),&operation);
		//--------
		tr_node datos;

		switch(operation){
			case 'T':
			//tamanio del archivo
				log_trace(logger,"Master %d: Solicitud de transformación recibida",socket_master);
				log_info(logger,"Master %d: Obteniendo datos de transformación",socket_master);

				//Obteniendo Script INFO
				readSocketBuffer(socket_master,sizeof(int),&(datos.fileSize));
				datos.file=malloc(datos.fileSize);
				readSocketBuffer(socket_master,datos.fileSize,datos.file);

				//Obteniendo Blocks INFO
				readSocketBuffer(socket_master,sizeof(int),&(datos.blocksSize));
				datos.blocks=malloc(sizeof(block)*datos.blocksSize);
				readSocketBuffer(socket_master,sizeof(block)*datos.blocksSize,datos.blocks);
				log_info(logger,"Master %d: Datos de transformación obtenidos",socket_master);

				char* scriptName = regenerateScript(datos.file,script_transform,operation,socket_master);

				//------------
				tr_node_rs* answer = malloc(sizeof(tr_node_rs));
				void* buff = malloc(sizeof(int)+sizeof(char)+sizeof(int));
				int i;
				for (i = 0; i < (datos.blocksSize); ++i){ //REEMPLAZAR POR FORKS
					answer->block = datos.blocks[i].pos;
					answer->result=transformBlock(datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp, scriptName);
					answer->runtime=12;
					//serializo
					memcpy(buff,&(answer->block),sizeof(int));
						//printf("\BLOQUE:%d\n", answer->block);
					memcpy(buff+sizeof(int),&(answer->result),sizeof(char));
						//printf("\RESULT:%c\n", answer->result);
					memcpy(buff+sizeof(int)+sizeof(char),&(answer->runtime),sizeof(int));
						//printf("\METRIC:%d\n", answer->runtime);
					send(socket_master,buff,sizeof(int)+sizeof(char)+sizeof(int),0);
				}
				free(buff);
				free(answer);

				//------------
				free(datos.blocks);
				free(datos.file);
				system("rm -f script.sh");			//borra el script temporal
				log_trace(logger, "Master %d: Transformación finalizada", socket_master);
				break;
			case 'L':
				log_info(logger,"Master %d: Solicitud de Reducción Local recibida");
				//reduceFile(file[],reductionScript);
				break;
			case 'G':
				log_info(logger,"Master %d: Solicitud de Reducción Global recibida");
				//globalReduction(files[],reductionScript);
				break;
			case 'S':
				log_info(logger,"Master %d: Solicitud de Almacenado Final recibida");
				//finalStorage(???,???);
				break;
			default:
				log_error(logger,"No existe la operación Solicitada");
				close(socket_master);
				break;
				//cerrar conexion y devolver error
		}
		//free(buffer);
}
