/*
 * localReduction.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"
#include <tplibraries/protocol/master_yama.h>

//=============YAMA_REQUEST=============================//
local_rs* sendLRequest(){

//PREPARO PAQUETE
	local_rq* data = malloc(sizeof(local_rq));
	void* buffer = malloc(sizeof(char));

//SERIALIZO
	data->code='L';
	memcpy(buffer,&(data->code),1);
	free(data);

//ENVIO REQUEST
	log_info(logger,"Envio solicitud de Reducción Local a YAMA");
	if(send(masterSocket,buffer,sizeof(char),0)<0){
		log_error(logger,"No se pudo conectar a YAMA");
	};
	free(buffer);

//RECIBO RESPONSE
	int i=0;
	local_rs* yamaAnswer=malloc(sizeof(local_rs));

	readBuffer(masterSocket, sizeof(char), &(yamaAnswer->code));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksQuantity));
	yamaAnswer->blockData = malloc(sizeof(rl_datos)*(yamaAnswer->blocksQuantity));

	for(i=0;i<(yamaAnswer->blocksQuantity);++i){
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].nodo));
		readBuffer(masterSocket, 16, &(yamaAnswer->blockData[i].ip));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].port));
		readBuffer(masterSocket, 28, &(yamaAnswer->blockData[i].tr_tmp));
		readBuffer(masterSocket, 28, &(yamaAnswer->blockData[i].rl_tmp));
		printf("\tnodo:%d\ttr_tmp:%s\trl_tmp:%s\tip:%s\t:port:%d\n", yamaAnswer->blockData[i].nodo, yamaAnswer->blockData[i].tr_tmp, yamaAnswer->blockData[i].rl_tmp,yamaAnswer->blockData[i].ip,yamaAnswer->blockData[i].port);
	};
	log_info(logger, "Datos de reducción local obtenidos de YAMA");
	return yamaAnswer;
}



//=============THREAD_ACTION===================================//

void *runLocalRedThread(void* data){
	dataThread_LR* datos = (dataThread_LR*) data;
	int i=0, counter=0;
	char* scriptString;

//PREPARO PAQUETE---
	scriptString=serializeFile(script_reduction);
	rl_node* nodeData = malloc(sizeof(rl_node));

	nodeData->code='L';
	nodeData->fileSize=strlen(scriptString)+1;
	nodeData->file=malloc(nodeData->fileSize);
	strcpy(nodeData->file,scriptString);
	strcpy(nodeData->rl_tmp, datos->rl_tmp);
	nodeData->tmpsQuantity = datos->tmpsCounter;

//SERIALIZO---
	int bufferSize = sizeof(char)+sizeof(int)*2+(nodeData->fileSize)+28+(nodeData->tmpsQuantity)*28;
	void* buffer = malloc(bufferSize);

	memcpy(buffer,&(nodeData->code),sizeof(char));									//Codigo de operación
		counter=sizeof(char);
	memcpy(buffer+counter,&(nodeData->fileSize),sizeof(int));						//Tamaño del script de Reduccion
		counter+=sizeof(int);
	memcpy(buffer+counter,(nodeData->file),nodeData->fileSize);						//Contenido delscript de Reduccion
		counter+=nodeData->fileSize;
	memcpy(buffer+counter,nodeData->rl_tmp,28);							//nombre el TMP de reducción
		counter+=28;
	memcpy(buffer+counter,&(nodeData->tmpsQuantity),sizeof(int));					//cantidad de TMPs a procesar
		counter+=sizeof(int);
	for(i = 0; i < (datos->tmpsCounter); ++i){
		memcpy(buffer+counter+(i*28),&(datos->tr_tmps[i]),28);	//TMPs de transformación
		//printf("\nTMPSEND:%s NODE:%d\n",(char*)buffer+counter+(i*28), datos->node);
	}
	counter+=(datos->tmpsCounter)*28;

//ENVÍO A WORKER (Transform ya abrió el socket)

	log_info(logger,"Estableciendo conexión con nodo %d",datos->node);
	if(send(nodeSockets[datos->node],buffer,bufferSize,0)<0){
		log_error(logger,"No se pudo conectar con nodo %d (%s:%d)", datos->node, datos->ip, datos->port);
		sendErrorToYama('L',datos->node);
		exit(1); //Ver como hacer para liberar memoria y cerrar hilos
	};

	log_info(logger,"Datos enviados a Nodo %d", datos->node);
	free(nodeData->file);
	free(nodeData);
	free(buffer);

//ESPERO RESPUESTA
	log_info(logger,"Esperando respuesta de nodo %d",datos->node);
	rl_node_rs* answer = malloc(sizeof(rl_node_rs));
	readBuffer(nodeSockets[datos->node], sizeof(char), &(answer->result));
	readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->runTime));
	//printf("RESULT:%cRUNTIME:%d\n",answer->result,answer->runTime);
//RESPONDO A YAMA
	log_trace(logger,"Nodo %d: Reducción Local Finalizada", datos->node);
	if(answer->result == 'O'){
		log_info(logger,"Comunicando a YAMA finalización de Reducción Local en nodo %d",datos->node);
		sendOkToYama('L',0,datos->node);
	}else{
		log_error(logger,"No fue posible realizar la Reducción Local en el nodo %d",datos->node);
		log_info(logger, "Se informa el error a YAMA");
		sendErrorToYama('L',datos->node);
	}

	/*
	printf("hilo iniciado:%d\t ip:%s\t port:%d\t reduciónFile:%s\n",datos[0].node,datos[0].ip,datos[0].port,datos[0].rl_tmp);
	for (i = 0; i <= (datos[0].tmpsCounter); ++i) printf("\t nodo:%d \t local:%s\n", datos[0].node, datos[0].tr_tmps[i]);
	*/

	return NULL;
}

//=============MAIN===================================//

int runLocalReduction(){
	struct timeval lr_start,lr_end;
	gettimeofday(&lr_start,NULL);
	int tmpsCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo, totalRecords;
	tr_tmp* tr_tmps = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_LR *dataThreads = NULL;				//creo array de params para el hilo
	local_rs* yamaAnswer;

	//Obtengo respuesta de YAMA
	yamaAnswer = sendLRequest();
	totalRecords = yamaAnswer->blocksQuantity;

	//Obtengo datos de los bloques
	int itemsSize = sizeof(rl_datos)*(yamaAnswer->blocksQuantity);
	rl_datos* items = malloc(itemsSize);
	memcpy(items,yamaAnswer->blockData,itemsSize);

	int i=0;
	for(i=0;i<(yamaAnswer->blocksQuantity);++i)
	printf("nodo:%d\ttr_tmp:%s\tip:%s\t:port:%d\n", items[i].nodo, items[i].tr_tmp,items[i].ip,items[i].port);
	/*
	*/

	while(recordCounter<totalRecords){
		nodo = items[recordCounter].nodo;	//init first key
		while(items[recordCounter].nodo==nodo && recordCounter<totalRecords){
			tr_tmps=(tr_tmp *) realloc(tr_tmps,(sizeof(tr_tmp)*(tmpsCounter+1)));
			strcpy(tr_tmps[tmpsCounter],items[recordCounter].tr_tmp);
			tmpsCounter++;
			recordCounter++;
		};
		//genero el hilo del nodo
		threads=(pthread_t *) realloc(threads,(sizeof(threads)*(nodeCounter+1)));
		dataThreads=(dataThread_LR *) realloc (dataThreads,(sizeof(dataThread_LR)*(nodeCounter+1)));
		//defino los parámetros
		//copio los bloques del nodo para no perderlos
		dataThreads[nodeCounter].tr_tmps=(tr_tmp *) malloc(sizeof(tr_tmp)*(tmpsCounter));
		dataThreads[nodeCounter].tr_tmps=(tr_tmp *) memcpy(dataThreads[nodeCounter].tr_tmps,tr_tmps, sizeof(tr_tmp)*(tmpsCounter));
		dataThreads[nodeCounter].node=nodo;
		strcpy(dataThreads[nodeCounter].ip,items[recordCounter-1].ip);
		dataThreads[nodeCounter].port=items[recordCounter-1].port;
		strcpy(dataThreads[nodeCounter].rl_tmp,items[recordCounter-1].rl_tmp);

		dataThreads[nodeCounter].tmpsCounter=tmpsCounter;
		nodeCounter++;
		tmpsCounter = 0;
	}

//EJECUTO HILOS DE WORKERS
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		usleep(10000);
		pthread_create(&threads[threadIndex],NULL,(void*) runLocalRedThread, (void*) &dataThreads[threadIndex]);
	}

//JOINEO LOS HILOS
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_join(threads[threadIndex],NULL);

//LIBERO MEMORIA
	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].tr_tmps);

	free(tr_tmps);
	free(dataThreads);
	free(threads);

	free(items);
	log_trace(logger,"REDUCCION LOCAL FINALIZADA");
	free(yamaAnswer);
	gettimeofday(&lr_end,NULL);
	masterMetrics.localReduction.runTime = timediff(&lr_end,&lr_start);
	return EXIT_SUCCESS;
}
