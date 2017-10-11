/*
 * finalStorage.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/finalStorage.h"

//=============YAMA_REQUEST=============================//
void sendSRequest(){

//---Preparo Paquete---
	store_rq* data;
	data = malloc(sizeof(store_rq));
	data->code='S';

//---Serializo---
	void* buffer = malloc(sizeof(char));
	memcpy(buffer,&(data->code),1);

//---Envío---
	send(masterSocket,buffer,sizeof(char),0);

	free(buffer);
	free(data);
}


int saveResult(af_datos yamaAnswer[], int totalRecords, metrics *masterMetrics){

	struct timeval af_start,af_end;
	gettimeofday(&af_start,NULL);

	sendSRequest();
	//conectarseConWorker y pasar yamaAnswer.rg_tmp

	gettimeofday(&af_end,NULL);
	masterMetrics->finalStorage.runTime = timediff(&af_end,&af_start);
	masterMetrics->finalStorage.errors = 0;

	return EXIT_SUCCESS;
}
