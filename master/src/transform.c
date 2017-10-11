/*
 * transform.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/transform.h"

//=============YAMA_REQUEST=============================//
void sendRequest(char* fileName){

//---Preparo Paquete---
	transform_rq* data;
	data = malloc(sizeof(transform_rq));
	data->code='T';
	data->fileNameSize= strlen(fileName)+1;
	data->fileName = malloc(data->fileNameSize);
	strcpy(data->fileName,fileName);

//---Serializo---
	void* buffer = malloc(5+(data->fileNameSize));
	memcpy(buffer,&(data->code),1);
	memcpy(buffer+1,&(data->fileNameSize),4);
	memcpy(buffer+5,(data->fileName),(data->fileNameSize));

//---Envío---
	send(masterSocket,buffer,5+(data->fileNameSize),0);

/*---DES_SERIALIZACIÓN---
	transform_rq *reciv = malloc(sizeof(transform_rq));
	memcpy(&(reciv->code),buffer,1);
	memcpy(&(reciv->fileNameSize),buffer+1,4);
	reciv->fileName = malloc (reciv->fileNameSize);
	strcpy(reciv->fileName, buffer+5);
	free(reciv->fileName);
	free(reciv);
*/
	free(buffer);
	free(data->fileName);
	free(data);
}

//=============THREAD_ACTION===================================//

void *runTransformThread(void* data){
/*
	int i;
	dataThread_TR* datos = (dataThread_TR*) data;
	printf("hilo iniciado:%d\n servidor:%s\n",datos[0].node,datos[0].conector);
	for (i = 0; i <= (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
*/
	return NULL;
}

//==============MAIN===========================================//

int transformFile(tr_datos yamaAnswer[], int totalRecords, metrics *masterMetrics, char* filename){
	struct timeval tr_start,tr_end;
	gettimeofday(&tr_start,NULL);
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_TR *dataThreads = NULL;				//creo array de params para el hilo

	sendRequest(filename);

	while(recordCounter<totalRecords){
		nodo = yamaAnswer[recordCounter].nodo;	//init first key
		while(yamaAnswer[recordCounter].nodo==nodo && recordCounter<totalRecords){
			blocks=(block *) realloc(blocks,(sizeof(block)*(blockCounter+1)));
			blocks[blockCounter].pos = yamaAnswer[recordCounter].bloque;
			blocks[blockCounter].size = yamaAnswer[recordCounter].tamanio;
			blockCounter++;
			recordCounter++;
		};

		//genero el hilo del nodo
		threads=(pthread_t *) realloc(threads,(sizeof(threads)*(nodeCounter+1)));
		dataThreads=(dataThread_TR *) realloc (dataThreads,(sizeof(dataThread_TR)*(nodeCounter+1)));

		//defino los parámetros
		dataThreads[nodeCounter].blocks=(block *) malloc(sizeof(block)*(blockCounter));
		dataThreads[nodeCounter].blocks=(block *) memcpy(dataThreads[nodeCounter].blocks,blocks, sizeof(block)*(blockCounter));

		dataThreads[nodeCounter].node=nodo;
		strcpy(dataThreads[nodeCounter].conector,yamaAnswer[recordCounter-1].direccion);
		dataThreads[nodeCounter].blocksCount=blockCounter-1;
		nodeCounter++;
		blockCounter = 0;
	}

//---creo los hilos------------
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_create(&threads[threadIndex],NULL,(void*) runTransformThread, (void*) &dataThreads[threadIndex]);

//---joineo los hilos----

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_join(threads[threadIndex],NULL);

//---libero memoria-----------------
	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].blocks);

	free(blocks);
	free(dataThreads);
	free(threads);

	gettimeofday(&tr_end,NULL);
	masterMetrics->transformation.runTime = timediff(&tr_end,&tr_start);

	return EXIT_SUCCESS;
}
