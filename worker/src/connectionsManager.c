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
		int bytesRecibidos = recv(socket_master, buffer, 1,0);
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

		switch(operation){
			case 'T':
				//recorrer y generar un fork por cada bloque
				//cada transformBlock va a hacer el send a Master

				log_info(logger,"Solicitud de transformación recibida");
				transformBlock(position,size,temporal);
				//}
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
