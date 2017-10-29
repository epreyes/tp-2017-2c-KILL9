/*
 * finalStorage.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/finalStorage.h"

//=============YAMA_REQUEST=============================//
store_rs* sendSRequest(){

//PREPARO PAQUETE
	store_rq* data = malloc(sizeof(store_rq));
	data->code='S';
//SERIALIZO
	void* buffer = malloc(sizeof(char));
	memcpy(buffer,&(data->code),sizeof(char));
//ENVIO REQUEST
	log_info(logger,"Envio solicitud de almacenamiento final a YAMA");
	send(masterSocket,buffer,sizeof(char),0);
	//if -1 abortar
	free(buffer);
	free(data);

//RECIBO RESPONSE
	store_rs* yamaAnswer=malloc(sizeof(store_rs));

	readBuffer(masterSocket, sizeof(char), &(yamaAnswer->code));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->nodo));
	readBuffer(masterSocket, 16, &(yamaAnswer->ip));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->port));
	readBuffer(masterSocket, 24, &(yamaAnswer->rg_tmp));

	printf("\tnodo:%c\tip:%s\tport:%d\ttmp:%s\n", yamaAnswer->nodo, yamaAnswer->ip, yamaAnswer->port,yamaAnswer->rg_tmp);

	return yamaAnswer;
	//recordar liberar
}




int sendDataToWorker(store_rs* datos, char* fileName){
//PREPARO PAQUETE---
	fs_node_rq* nodeData = malloc(sizeof(fs_node_rq));

	nodeData->code='S';										//CODE
	strcpy(nodeData->rg_tmp, datos->rg_tmp);			//NOMBRE TMP RED GLOBAL
	nodeData->fileNameSize = strlen(fileName);				//NOMBRE ARCHIVO FINAL
	nodeData->fileName = malloc(nodeData->fileNameSize);
	strcpy(nodeData->fileName, fileName);

//SERIALIZO---
	int bufferSize = sizeof(char)+24+sizeof(int)+nodeData->fileNameSize;
	void* buffer = malloc(bufferSize);

	memcpy(buffer,&(nodeData->code),sizeof(char));
	memcpy(buffer+sizeof(char),&(nodeData->rg_tmp), 24);
	memcpy(buffer+sizeof(char)+24,&(nodeData->fileNameSize),sizeof(int));
	memcpy(buffer+sizeof(char)+24+sizeof(int),(nodeData->fileName),nodeData->fileNameSize);

	free(nodeData->fileName);
	free(nodeData);

//ENVÍO A WORKER
	log_info(logger,"Estableciendo conexión con nodo %d",datos->nodo);
	if(send(nodeSockets[datos->nodo],buffer,bufferSize,0)<0){
		sendErrorToYama('S',datos->nodo);
		free(buffer);
		return 1;
	};
	free(buffer);

//ESPERO RESPUESTA
	log_info(logger,"Esperando respuesta del Nodo Encargado");
	fs_node_rs* answer = malloc(sizeof(fs_node_rs));
	readBuffer(nodeSockets[datos->nodo], sizeof(int), &(answer->runTime));
	readBuffer(nodeSockets[datos->nodo], sizeof(char), &(answer->result));

//RESPONDO A YAMA
	if(answer->result == 'O'){
		log_trace(logger,"Almacenado Final Realizado por nodo %d", datos->nodo);
		log_info(logger,"Se informa a YAMA la finalización exitosa del JOB",datos->nodo);
		if(sendOkToYama('S',0,datos->nodo));
		free(answer);
		return 0;
	}else{
		log_error(logger,"El nodo %d no pudo realizar el almacenado final",datos->nodo);
		log_info(logger, "Se informa el error a YAMA ");
		sendErrorToYama('S',datos->nodo);
		free(answer);
		return 1;
	}
}



int saveResult(metrics *masterMetrics, char* fileName){


	struct timeval af_start,af_end;
	gettimeofday(&af_start,NULL);

	log_trace(logger,"INICIANDO PROCESO DE ALMACENADO FINAL");

	store_rs* yamaAnswer=NULL;
	yamaAnswer = sendSRequest();
	sendDataToWorker(&yamaAnswer, fileName);

	//GUARDO METRICA
	gettimeofday(&af_end,NULL);
	masterMetrics->finalStorage.runTime = timediff(&af_end,&af_start);
	masterMetrics->finalStorage.errors = 0;

	free(yamaAnswer);
	return EXIT_SUCCESS;
}
