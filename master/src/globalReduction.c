/*
 * globalReduction.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"

//=============YAMA_REQUEST=============================//
void sendGRequest(){
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
/*
 * typedef struct{
	char		code;
	int			bocksQuantity;
	rl_datos*	blocksData;
}global_rs;

	typedef struct rg_datos{				//es un record por nodo
		int		nodo;
		char	ip[16];
		int 	port;
		char	rl_tmp[28];
		char	rg_tmp[24];
		char	encargado;
	}rg_datos;
*/

//RECIBO RESPONSE
/*
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
*/
}




////////////////////////////////////GLOBAL_REDUCTION/////////////////////////////////////////

int runGlobalReduction(metrics *masterMetrics){
	struct timeval gr_start,gr_end;
	gettimeofday(&gr_start,NULL);
	int totalRecords;

	int recordCounter=0;
	dataNodes *brothersData = NULL;
	brothersData = (dataNodes*) malloc(sizeof(dataNodes));
	dataThread_GR* dataThread = NULL;
	dataThread = (dataThread_GR*) malloc(sizeof(dataThread_GR));
	rg_datos* yamaAnswer;
/*
	yamaAnswer = sendGRequest();
	totalRecords = yamaAnswer->
*/
	//recorro los registros de la respuesta
	while(recordCounter<totalRecords){
		if(yamaAnswer[recordCounter].encargado==1){
			dataThread->leadNode = yamaAnswer[recordCounter].nodo;
			strcpy(dataThread->leadIp, yamaAnswer[recordCounter].ip);
			dataThread->leadPort = yamaAnswer[recordCounter].port;
			strcpy(dataThread->rg_tmp, yamaAnswer[recordCounter].rg_tmp);
		}else{
			brothersData = (dataNodes*)realloc(brothersData,(sizeof(dataNodes)*(recordCounter+1)));
			strcpy(brothersData[recordCounter].ip, yamaAnswer[recordCounter].ip);
			brothersData[recordCounter].port = yamaAnswer[recordCounter].port;
			brothersData[recordCounter].node = yamaAnswer[recordCounter].nodo;
			strcpy(brothersData[recordCounter].rl_tmp, yamaAnswer[recordCounter].rl_tmp);
		}
		recordCounter++;
	};

	dataThread->brothersData = (dataNodes *) malloc(sizeof(dataNodes)*recordCounter);
	dataThread->brothersData = (dataNodes *) memcpy(dataThread->brothersData,brothersData, sizeof(dataNodes)*recordCounter);
	dataThread->brothersCount = recordCounter;


	free(brothersData);
	free(dataThread->brothersData);
	free(dataThread);

	gettimeofday(&gr_end,NULL);
	masterMetrics->globalReduction.runTime = timediff(&gr_end,&gr_start);

	return EXIT_SUCCESS;
}
