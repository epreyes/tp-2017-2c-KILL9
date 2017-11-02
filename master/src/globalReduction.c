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
	};
	free(buffer);
/* typedef struct{
	char		code;
	int			blocksQuantity;
	rg_datos*	blocksData;
}global_rs;

typedef struct rg_datos{				//es un record por nodo
	int		nodo;
	char	ip[16];
	int 	port;
	char	rl_tmp[28];
	char	rg_tmp[24];
	char	encargado;
}rg_datos; */

//RECIBO RESPONSE
	int i=0;
	global_rs* yamaAnswer=malloc(sizeof(global_rs));

	readBuffer(masterSocket, sizeof(char), &(yamaAnswer->code));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksQuantity));
	yamaAnswer->blocksData = malloc(sizeof(rg_datos)*(yamaAnswer->blocksQuantity));

	for(i=0;i<(yamaAnswer->blocksQuantity);++i){
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksData[i].nodo));
		readBuffer(masterSocket, 16, &(yamaAnswer->blocksData[i].ip));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blocksData[i].port));
		readBuffer(masterSocket, 28, &(yamaAnswer->blocksData[i].rl_tmp));
		readBuffer(masterSocket, 24, &(yamaAnswer->blocksData[i].rg_tmp));
		readBuffer(masterSocket, sizeof(char),&(yamaAnswer->blocksData[i].encargado));
		printf("\tnodo:%d\ttr_tmp:%s\trl_tmp:%s\tip:%s\t:port:%d\n", yamaAnswer->blocksData[i].nodo, yamaAnswer->blocksData[i].rg_tmp, yamaAnswer->blocksData[i].rl_tmp,yamaAnswer->blocksData[i].ip,yamaAnswer->blocksData[i].port);
	};
	log_info(logger, "Datos de Reducción local obtenidos de YAMA");
	return yamaAnswer;
}


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

	for(i=0;i<nodeData->nodesQuantity;++i){
		nodeData->nodes[i].node = datos->brothersData->node;
		strcpy(nodeData->nodes[i].ip,datos->brothersData[i].ip);
		nodeData->nodes[i].port = datos->brothersData[i].port;
		strcpy(nodeData->nodes[i].rl_tmp,datos->brothersData[i].rl_tmp);
	};
//SERIALIZO---
	int bufferSize = sizeof(char)+sizeof(int)*2+(nodeData->fileSize)+28+24+(nodeData->nodesQuantity)*sizeof(rg_node);
	void* buffer = malloc(bufferSize);

	memcpy(buffer,&(nodeData->code),sizeof(char));									//Codigo de operación
		counter=sizeof(char);
	memcpy(buffer+counter,&(nodeData->fileSize),sizeof(int));						//Tamaño del script de Reduccion
		counter+=sizeof(int);
	memcpy(buffer+counter,(nodeData->file),nodeData->fileSize);						//Contenido delscript de Reduccion
		counter+=nodeData->fileSize;
	memcpy(buffer+counter,nodeData->rl_tmp,28);							//nombre el TMP de reducción
		counter+=28;
	memcpy(buffer+counter,nodeData->rg_tmp,24);							//nombre el TMP de reducción
		counter+=24;
	memcpy(buffer+counter,&(nodeData->nodesQuantity),sizeof(int));					//cantidad de TMPs a procesar
		counter+=sizeof(int);
	for(i = 0; i < (nodeData->nodesQuantity); ++i){
		memcpy(buffer+counter,&(nodeData->nodes[i].node),sizeof(int));
		counter+=sizeof(int);
		memcpy(buffer+counter,&(nodeData->nodes[i].ip),16);
		counter+=16;
		memcpy(buffer+counter,&(nodeData->nodes[i].port),sizeof(int));
		counter+=sizeof(int);
		memcpy(buffer+counter,(nodeData->nodes[i].rl_tmp),28);
		counter+=28;
	}

//ENVÍO A WORKER (Transform ya abrió el socket)

	log_info(logger,"Estableciendo conexión con nodo %d",datos->leadNode);
	if(send(nodeSockets[datos->leadNode],buffer,bufferSize,0)<0){
		log_error(logger,"No se pudo conectar con nodo %d (%s:%d)", datos->leadNode, datos->leadIp, datos->leadPort);
		sendErrorToYama('G',datos->leadNode);
		exit(1); //Ver como hacer para liberar y cancelar el programa
	};

	log_info(logger,"Datos enviados a Nodo %d", datos->leadNode);


	free(buffer);
	free(nodeData->nodes);
	free(nodeData->file);
	free(nodeData);

//ESPERO RESPUESTA
	log_info(logger,"Esperando respuesta de nodo %d",datos->leadNode);
	rg_node_rs* answer = malloc(sizeof(rg_node_rs));
	readBuffer(nodeSockets[datos->leadNode], sizeof(char), &(answer->result));
	readBuffer(nodeSockets[datos->leadNode], sizeof(int), &(answer->runTime));
	//printf("RESULT:%cRUNTIME:%d\n",answer->result,answer->runTime);
//RESPONDO A YAMA
	log_trace(logger,"Nodo %d: Reducción Local Finalizada", datos->leadNode);
	if(answer->result == 'O'){
		log_info(logger,"Comunicando a YAMA finalización de Reducción Local en nodo %d",datos->leadNode);
		sendOkToYama('G',0,datos->leadNode);
	}else{
		log_error(logger,"No fue posible realizar la Reducción Global en el nodo %d",datos->leadNode);
		log_info(logger, "Se informa el error a YAMA");
		sendErrorToYama('G',datos->leadNode);
	}

	printf("RG iniciada:%d\t ip:%s:%d\t RL:%s\t RG:%s\n",datos->leadNode,datos->leadIp,datos->leadPort,datos->rl_tmp,datos->rg_tmp);
	for (i = 0; i <= (datos->brothersCount); ++i) printf("\t nodo:%d \t local:%s\n", datos->brothersData[i].node, datos->brothersData[i].rl_tmp);

/*
*/

	return 0;
};

////////////////////////////////////GLOBAL_REDUCTION/////////////////////////////////////////

int runGlobalReduction(metrics *masterMetrics){
	struct timeval gr_start,gr_end;
	gettimeofday(&gr_start,NULL);
	int totalRecords, recordCounter=0;
	dataNodes *brothersData = NULL;
	brothersData = (dataNodes*) malloc(sizeof(dataNodes));

	dataThread_GR* dataThread = NULL;
	dataThread = (dataThread_GR*) malloc(sizeof(dataThread_GR));
	global_rs* yamaAnswer;
	yamaAnswer = sendGRequest();
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
		}else{
			dataThread->brothersData = realloc(dataThread->brothersData,(sizeof(dataNodes)*(recordCounter+1)));
			strcpy(dataThread->brothersData[recordCounter].ip, items[recordCounter].ip);
			dataThread->brothersData[recordCounter].port = items[recordCounter].port;
			dataThread->brothersData[recordCounter].node = items[recordCounter].nodo;
			strcpy(brothersData[recordCounter].rl_tmp, items[recordCounter].rl_tmp);
		}
		recordCounter++;
	};

	dataThread->brothersCount = recordCounter;

	sendNodeRequest(dataThread);

	free(brothersData);
	free(dataThread->brothersData);
	free(dataThread);

	free(yamaAnswer);
	free(items);

//METRICS================================================================
	gettimeofday(&gr_end,NULL);
	masterMetrics->globalReduction.runTime = timediff(&gr_end,&gr_start);

	return EXIT_SUCCESS;
}
