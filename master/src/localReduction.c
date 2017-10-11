/*
 * localReduction.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"

//=============YAMA_REQUEST=============================//
void sendLRequest(){

//---Preparo Paquete---
	local_rq* data;
	data = malloc(sizeof(local_rq));
	data->code='L';

//---Serializo---
	void* buffer = malloc(sizeof(char));
	memcpy(buffer,&(data->code),1);

//---Envío---
	send(masterSocket,buffer,sizeof(char),0);

	free(buffer);
	free(data);
}

//=============THREAD_ACTION===================================//

void *runLocalRedThread(void* data){
	int i;
	dataThread_LR* datos = (dataThread_LR*) data;
	printf("hilo iniciado:%d\t servidor:%s\t reduciónFile:%s\n",datos[0].node,datos[0].conector,datos[0].rl_tmp);
	for (i = 0; i <= (datos[0].tmpsCounter); ++i){
		printf("\t nodo:%d \t local:%s\n", datos[0].node, datos[0].tr_tmps[i]);
	}
	//conectarse con WORKER y esperar respuesta.
	return NULL;
}


int runLocalReduction(rl_datos yamaAnswer[], int totalRecords, metrics *masterMetrics){
	struct timeval lr_start,lr_end;
	gettimeofday(&lr_start,NULL);
	int tmpsCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	tr_tmp* tr_tmps = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_LR *dataThreads = NULL;				//creo array de params para el hilo

	sendLRequest();

	while(recordCounter<totalRecords){
		nodo = yamaAnswer[recordCounter].nodo;	//init first key
		while(yamaAnswer[recordCounter].nodo==nodo && recordCounter<totalRecords){
			tr_tmps=(tr_tmp *) realloc(tr_tmps,(sizeof(tr_tmp)*(tmpsCounter+1)));
			strcpy(tr_tmps[tmpsCounter],yamaAnswer[tmpsCounter].tr_tmp);
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
		strcpy(dataThreads[nodeCounter].conector,yamaAnswer[recordCounter-1].direccion);
		strcpy(dataThreads[nodeCounter].rl_tmp,yamaAnswer[recordCounter-1].rl_tmp);
		dataThreads[nodeCounter].tmpsCounter=tmpsCounter-1;
		nodeCounter++;
		tmpsCounter = 0;
	}

//------creo los hilos------------
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_create(&threads[threadIndex],NULL,(void*) runLocalRedThread, (void*) &dataThreads[threadIndex]);

//------joineo los hilos----

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_join(threads[threadIndex],NULL);

//------libero memoria-----------------

	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].tr_tmps);

	free(tr_tmps);
	free(dataThreads);
	free(threads);

	gettimeofday(&lr_end,NULL);
	masterMetrics->localReduction.runTime = timediff(&lr_end,&lr_start);
	return EXIT_SUCCESS;
}
