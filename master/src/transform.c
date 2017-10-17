/*
 * transform.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/transform.h"

//=============YAMA_REQUEST=============================//
void sendTRequest(char* fileName){

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
	//send(masterSocket,buffer,5+(data->fileNameSize),0);

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
	dataThread_TR* datos = (dataThread_TR*) data;
	int i, counter=0;
	char* scriptString;

//---Preparo Paquete---

	scriptString=serializeFile(script_transform);	//hacer que se genere una sola vez
	tr_node* nodeData=malloc(sizeof(tr_node));
	nodeData->code='T';
	nodeData->fileSize=strlen(scriptString)+1;
	nodeData->file=malloc(nodeData->fileSize);		//ver el +1
	nodeData->blocksSize=datos->blocksCount;
	strcpy(nodeData->file,scriptString);

//---Serializo---
	void* buffer = malloc(1+4+nodeData->fileSize+4+(36*(datos->blocksCount)));
	memcpy(buffer,&(nodeData->code),1);
	memcpy(buffer+1,&(nodeData->fileSize),4);
	memcpy(buffer+1+4,(nodeData->file),nodeData->fileSize);
	memcpy(buffer+1+4+(nodeData->fileSize),&(nodeData->blocksSize),4);
	counter=1+4+(nodeData->fileSize)+4;

	for (i = 0; i < (datos->blocksCount); ++i){
		memcpy(buffer+counter+i*36,&(datos->blocks[i].pos),4);
		memcpy(buffer+counter+4+i*36,&(datos->blocks[i].size),4);
		memcpy(buffer+counter+4+i*36,(datos->blocks[i].tmp),28);
	}
	//openNodeConnection(datos[0].node, datos[0].conector);
	counter=1+4+(nodeData->fileSize)+4+(36*(datos->blocksCount));

//---Envío---
	//send(nodeSockets[datos->node],buffer,counter,0);

/*
//---DES_SERIALIZACIÓN---
	tr_node *reciv = malloc(sizeof(tr_node));
	memcpy(&(reciv->code),buffer,1);
	memcpy(&(reciv->fileSize),buffer+1,4);

	reciv->file = malloc(reciv->fileSize);
	strcpy(reciv->file, buffer+5);

	memcpy(&(reciv->blocksSize), buffer+5+(reciv->fileSize),4);

	reciv->blocks = malloc(sizeof(block)*reciv->blocksSize);
	memcpy(reciv->blocks, buffer+5+(reciv->fileSize)+4,sizeof(block)*reciv->blocksSize);

	for (i = 0; i < reciv->blocksSize; ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d \t file:%s\n", datos->node, reciv->blocks[i].pos, reciv->blocks[i].size, reciv->blocks[i].tmp);
	}

	printf("CODE:%c\n",reciv->code);
	printf("FILESIZE:%d\n",reciv->fileSize);
	printf("FILE:%s\n",reciv->file);
	printf("BLOCKSIZE:%d\n", reciv->blocksSize);

	free(reciv->blocks);
	free(reciv->file);
	free(reciv);

	for (i = 0; i <= (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
*/
	free(buffer);
	free(nodeData->file);
	free(nodeData);
	free(scriptString);
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

	sendTRequest(filename);

	while(recordCounter<totalRecords){
		nodo = yamaAnswer[recordCounter].nodo;	//init first key
		while(yamaAnswer[recordCounter].nodo==nodo && recordCounter<totalRecords){
			blocks=(block *) realloc(blocks,(sizeof(block)*(blockCounter+1)));
			blocks[blockCounter].pos = yamaAnswer[recordCounter].bloque;
			blocks[blockCounter].size = yamaAnswer[recordCounter].tamanio;
			strcpy(blocks[blockCounter].tmp,yamaAnswer[recordCounter].tr_tmp);
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
		dataThreads[nodeCounter].blocksCount=blockCounter;
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

	free(dataThreads);
	free(threads);
	free(blocks);

	gettimeofday(&tr_end,NULL);
	masterMetrics->transformation.runTime = timediff(&tr_end,&tr_start);

	return EXIT_SUCCESS;
}
