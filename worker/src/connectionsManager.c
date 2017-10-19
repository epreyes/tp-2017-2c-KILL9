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
	int workerSocket = socket(AF_INET, SOCK_STREAM, 0);

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
		log_info(logger,"conexión de Master %d recibida",masterSocket);
	}else{
		log_error(logger,"error al establecer conexión");
		exit(1);
	}

//==========================================================================
	void* buffer = malloc(1);
	char operation;
	//resultados Hardcodeados//

	int bytesRecibidos = recv(masterSocket, buffer, 1,0);
	if (bytesRecibidos <= 0) {
		log_error(logger,"el master %d se encuentra desconectado");
		exit(1);
	}

	memcpy(&operation,buffer,1);
	printf("Código recibido:%c\n", operation);


/*
	char temporal[28];
	int position = 65;
	int size = 10000;
	switch(operation){
		case 'T':
			regenerateScript("hola mundo",script_transform,"tr.sh");
			//recorrer y generar un fork por cada bloque
			//cada transformBlock va a hacer el send a Master
			transformBlock(position,size,temporal);
			//}
			break;
		case 'L':
			regenerateScript("hola mundo",script_reduction,"rd.sh");
			//reduceFile(file[],reductionScript);
			break;
		case 'G':
			//globalReduction(files[],reductionScript);
			break;
		case 'S':
			//finalStorage(???,???);
			break;
	}
*/

	free(buffer);
}
