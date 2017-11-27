/*
 * globalReduction.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"
//=============YAMA_REQUEST=============================//
global_rs* sendGRequest(){
//EMPAQUETO
	global_rq* data = malloc(sizeof(global_rq));
	data->code='G';
//SERIALIZO
	void* buffer = malloc(sizeof(char));
	memcpy(buffer,&(data->code),1);
	free(data);
//ENVIO
	if(send(masterSocket,buffer,sizeof(char),0)<0){
		log_error(logger,"No se pudo conectar a YAMA");
		abortJob = 'G';
		free(buffer);
		return NULL;
	};
	free(buffer);

//RECIBO RESPONSE
	int i=0;
	global_rs* yamaAnswer=malloc(sizeof(global_rs));

	readBuffer(masterSocket, sizeof(char), &(yamaAnswer->code));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksQuantity));
	yamaAnswer->blocksData = malloc(sizeof(rg_datos)*(yamaAnswer->blocksQuantity));
	//printf("code:%c blocks:%d",yamaAnswer->code,yamaAnswer->blocksQuantity);

	for(i=0;i<(yamaAnswer->blocksQuantity);++i){
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksData[i].nodo));
		readBuffer(masterSocket, 16, &(yamaAnswer->blocksData[i].ip));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksData[i].port));
		readBuffer(masterSocket, 28, &(yamaAnswer->blocksData[i].rl_tmp));
		readBuffer(masterSocket, 24, &(yamaAnswer->blocksData[i].rg_tmp));
		readBuffer(masterSocket, sizeof(char),&(yamaAnswer->blocksData[i].encargado));
		printf("N:%d(%s:%d) RL:%s RG:%s E:%c\n\n", yamaAnswer->blocksData[i].nodo, yamaAnswer->blocksData[i].ip,yamaAnswer->blocksData[i].port,yamaAnswer->blocksData[i].rl_tmp,yamaAnswer->blocksData[i].rg_tmp,yamaAnswer->blocksData[i].encargado);
	};
	log_info(logger, "Datos de Reducción Global obtenidos de YAMA");
	return yamaAnswer;
}

//=============REPORT_ERROR======================================//
void reportGRError(int node){
	sendErrorToYama('G',node);
	abortJob = 'G';
	increaseMetricsError(&(masterMetrics.globalReduction.errors));
}

//==============THREAD==========================================//
int sendNodeRequest(dataThread_GR* datos){

	int i=0, counter=0;
	char* scriptString;

//PREPARO PAQUETE---
	scriptString = serializeFile(script_reduction);
	rg_node_rq* nodeData = malloc(sizeof(rg_node_rq));

	nodeData->code='G';
	nodeData->fileSize=strlen(scriptString)+1;
	nodeData->file=malloc(nodeData->fileSize);
	strcpy(nodeData->file,scriptString);
	strcpy(nodeData->rl_tmp, datos->rl_tmp);
	strcpy(nodeData->rg_tmp, datos->rg_tmp);
	nodeData->nodesQuantity = datos->brothersCount;

//SERIALIZO---
	int bufferSize = sizeof(char)+sizeof(int)*2+(nodeData->fileSize)+28+24+((nodeData->nodesQuantity)*sizeof(rg_node));
	void* buffer = malloc(bufferSize);

	memcpy(buffer,&(nodeData->code),sizeof(char));						//Codigo de operación
		counter=sizeof(char);
	memcpy(buffer+counter,&(nodeData->fileSize),sizeof(int));			//Tamaño del script de Reduccion
		counter+=sizeof(int);
	memcpy(buffer+counter,(nodeData->file),nodeData->fileSize);			//Contenido delscript de Reduccion
		counter+=nodeData->fileSize;
	memcpy(buffer+counter,nodeData->rl_tmp,28);							//nombre el TMP de reducción
		counter+=28;
	memcpy(buffer+counter,nodeData->rg_tmp,24);							//nombre el TMP de reducción
		counter+=24;
	memcpy(buffer+counter,&(nodeData->nodesQuantity),sizeof(int));		//cantidad de TMPs a procesar
		counter+=sizeof(int);
	for(i = 0; i < (datos->brothersCount); ++i){
		memcpy(buffer+counter,&(datos->brothersData[i].node),sizeof(int));
		counter+=sizeof(int);
		memcpy(buffer+counter,&(datos->brothersData[i].ip),16);
		counter+=16;
		memcpy(buffer+counter,&(datos->brothersData[i].port),sizeof(int));
		counter+=sizeof(int);
		memcpy(buffer+counter,(datos->brothersData[i].rl_tmp),28);
		counter+=28;
		printf("DATOS:%d %s %d %s\n",datos->brothersData[i].node,datos->brothersData[i].ip,datos->brothersData[i].port,datos->brothersData[i].rl_tmp);
	}


//ENVÍO A WORKER
	log_info(logger,"Estableciendo conexión con nodo %d(%s:%d)",datos->leadNode,datos->leadIp,datos->leadPort);
	if(send(nodeSockets[datos->leadNode],buffer,bufferSize,0)<0){
		log_error(logger,"No se pudo conectar con nodo %d (%s:%d)", datos->leadNode, datos->leadIp, datos->leadPort);
		reportGRError(datos->leadNode);
		return 1;
	};

	log_info(logger,"Datos enviados a Nodo %d", datos->leadNode);
	log_trace(logger,"Nodo %d: Reducción Global Iniciada", datos->leadNode);

	free(buffer);
	free(scriptString);
	free(nodeData->file);
	free(nodeData);
	//METRICS
	gettimeofday(&(datos->metrics.start),NULL);

//ESPERO RESPUESTA
	log_info(logger,"Esperando respuesta de nodo %d",datos->leadNode);
	rg_node_rs* answer = malloc(sizeof(rg_node_rs));
	readBuffer(nodeSockets[datos->leadNode], sizeof(char), &(answer->result));

	gettimeofday(&(datos->metrics.end),NULL);
	masterMetrics.globalReduction.runTime+= timediff(&(datos->metrics.end),&(datos->metrics.start));

//RESPONDO A YAMA
	log_trace(logger,"Nodo %d: Reducción Global Finalizada", datos->leadNode);
	if(answer->result == 'O'){
		log_info(logger,"Comunicando a YAMA finalización de Reducción Local en nodo %d",datos->leadNode);
		sendOkToYama('G',0,datos->leadNode);
	}else{
		log_error(logger,"No fue posible realizar la Reducción Global en el nodo %d",datos->leadNode);
		log_info(logger, "Se informa el error a YAMA");
		reportGRError(datos->leadNode);
		free(answer);
		return 1;
	}

	free(answer);
	return 0;
};

////////////////////////////////////GLOBAL_REDUCTION/////////////////////////////////////////

int runGlobalReduction(){
	int totalRecords=0, recordCounter=0, brothersCounter=0;

	dataThread_GR* dataThread = NULL;
	dataThread = (dataThread_GR*) malloc(sizeof(dataThread_GR));
	dataThread->brothersData = (dataNodes*) malloc(sizeof(dataNodes));

	global_rs* yamaAnswer;

	yamaAnswer = sendGRequest();

	if(!yamaAnswer){
		log_error(logger, "REDUCCION GLOBAL ABORTADA");
		return EXIT_FAILURE;
	};

	totalRecords = yamaAnswer->blocksQuantity;

	//Obtengo datos de los bloques
	int itemsSize = sizeof(rg_datos)*(yamaAnswer->blocksQuantity);
	rg_datos* items = malloc(itemsSize);
	memcpy(items,yamaAnswer->blocksData,itemsSize);			//PASAR A PUNTERO

//recorro los registros de la respuesta
	while(recordCounter<totalRecords){
		if(items[recordCounter].encargado=='T'){
			dataThread->leadNode = items[recordCounter].nodo;
			strcpy(dataThread->leadIp, items[recordCounter].ip);
			dataThread->leadPort = items[recordCounter].port;
			strcpy(dataThread->rg_tmp, items[recordCounter].rg_tmp);
			strcpy(dataThread->rl_tmp, items[recordCounter].rl_tmp);
			printf("n:%d (%s:%d) %s %s", dataThread->leadNode, dataThread->leadIp,dataThread->leadPort, dataThread->rg_tmp, dataThread->rl_tmp);
		}else{
			dataThread->brothersData = realloc(dataThread->brothersData,(sizeof(dataNodes)*(brothersCounter+1)));
			strcpy(dataThread->brothersData[brothersCounter].ip, items[recordCounter].ip);
			dataThread->brothersData[brothersCounter].port = items[recordCounter].port;
			dataThread->brothersData[brothersCounter].node = items[recordCounter].nodo;
			strcpy(dataThread->brothersData[brothersCounter].rl_tmp, items[recordCounter].rl_tmp);
			brothersCounter++;
		}
		recordCounter++;
	};
		//printf("\nDATOS:%d %s %d %s\n",dataThread->brothersData[0].node,dataThread->brothersData[0].ip,dataThread->brothersData[0].port,dataThread->brothersData[0].rl_tmp);

	dataThread->brothersCount = brothersCounter;

	sendNodeRequest(dataThread);

	free(dataThread->brothersData);
	free(dataThread);
	free(yamaAnswer->blocksData);
	free(yamaAnswer);
	free(items);

	if(abortJob=='0'){
		log_trace(logger, "REDUCCIÓN GLOBAL FINALIZADA");
		return EXIT_SUCCESS;
	}else{
		log_error(logger, "REDUCCIÓN GLOBAL ABORTADA");
		return EXIT_FAILURE;
	}
}
