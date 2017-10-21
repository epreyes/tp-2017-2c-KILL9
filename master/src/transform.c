/*
 * transform.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/transform.h"

//=============YAMA_REQUEST=============================//
transform_rs* sendTRequest(char* fileName){

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
	log_info(logger, "Solicitando datos de transformación a YAMA");
	send(masterSocket,buffer,5+(data->fileNameSize),0);

	free(buffer);
	free(data->fileName);
	free(data);

//---Esperando respuesta----
	int i=0;

	transform_rs* yamaAnswer=malloc(sizeof(transform_rs));

	readBuffer(masterSocket, sizeof(char), &(yamaAnswer->code));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->bocksQuantity));
	yamaAnswer->blockData = malloc(sizeof(tr_datos)*(yamaAnswer->bocksQuantity));

	for(i=0;i<(yamaAnswer->bocksQuantity);++i){
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].nodo));
		readBuffer(masterSocket, 16, &(yamaAnswer->blockData[i].ip));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].port));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].bloque));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].tamanio));
		readBuffer(masterSocket, 28, &(yamaAnswer->blockData[i].tr_tmp));
	}
//	free(yamaAnswer);

	return (yamaAnswer);
}

//=============THREAD_ACTION===================================//

void *runTransformThread(void* data){
	dataThread_TR* datos = (dataThread_TR*) data;
	int i, counter=0;
	char* scriptString;

//---Preparo Paquete---

	scriptString=serializeFile(script_transform);	//hacer que se genere una sola vez(?)
	tr_node* nodeData=malloc(sizeof(tr_node));
	nodeData->code='T';
	nodeData->fileSize=strlen(scriptString)+1;
	nodeData->file=malloc(nodeData->fileSize);		//ver el +1
	strcpy(nodeData->file,scriptString);
	nodeData->blocksSize=datos->blocksCount;

//---Serializo---
	void* buffer = malloc(sizeof(char)+sizeof(int)+nodeData->fileSize+sizeof(int)+(sizeof(block)*(datos->blocksCount)));
	memcpy(buffer,&(nodeData->code),sizeof(char));
	counter=sizeof(char);
	memcpy(buffer+counter,&(nodeData->fileSize),sizeof(int));
	counter+=sizeof(int);
	memcpy(buffer+counter,(nodeData->file),nodeData->fileSize);
	counter+=nodeData->fileSize;
	memcpy(buffer+counter,&(nodeData->blocksSize),sizeof(int));
	counter+=sizeof(int);

	for (i = 0; i < (datos->blocksCount); ++i){
		memcpy(buffer+counter+(i*sizeof(block)),&(datos->blocks[i].pos),sizeof(int));
		memcpy(buffer+counter+sizeof(int)+i*sizeof(block),&(datos->blocks[i].size),sizeof(int));
		memcpy(buffer+counter+sizeof(int)+sizeof(int)+i*sizeof(block),(datos->blocks[i].tmp),28);
	}
	log_info(logger,"Estableciendo conexión con nodo %d",datos->node);
	if(openNodeConnection(datos[0].node, datos[0].ip, datos[0].port)!=0){
//---Reporto error a YAMA
		error_rq* error = malloc(sizeof(error_rq));
		error->code='E';
		error->opCode = 'T';
		error->nodo=datos->node;

		void* buff=malloc(sizeof(char)*2+sizeof(int));
		memcpy(buff,&(error->code),sizeof(char));
		memcpy(buff+sizeof(char),&(error->opCode),sizeof(char));
		memcpy(buff+sizeof(char)+sizeof(char), &(error->nodo), sizeof(int));
		send(masterSocket,buff,sizeof(char)*2+sizeof(int),0);
		free(error);
		free(buff);
		log_warning(logger, "Reportado a YAMA, a la espera de replanificación");
	};


//--Envío---
	counter=1+4+(nodeData->fileSize)+4+(36*(datos->blocksCount));
	send(nodeSockets[datos->node],buffer,counter,0);
	log_trace(logger,"Nodo %d: Transformación iniciada", datos->node);

//--Espero respuesta

	tr_node_rs* answer = malloc(sizeof(tr_node_rs));

	for(i=0;i < (datos->blocksCount); ++i){
		//-----
		readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->block));
			printf("BLOQUE:%d\n",answer->block);
		readBuffer(nodeSockets[datos->node], sizeof(char), &(answer->result));
			printf("RESULT:%c\n",answer->result);
		readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->runtime));
			printf("METRIC:%d\n",answer->runtime);

		//respuesta a YAMA
		ok* answerToYama = malloc(sizeof(ok));
		void* buffYama = malloc(sizeof(char)+sizeof(char)+sizeof(int)+sizeof(int));
		answerToYama->code='O';
		answerToYama->opCode='T';
		answerToYama->bloque=answer->block;
		answerToYama->nodo=datos->node;

		memcpy(buffYama,&(answerToYama->code),sizeof(char));
		memcpy(buffYama+sizeof(char),&(answerToYama->opCode),sizeof(char));
		memcpy(buffYama+2*sizeof(char),&(answerToYama->bloque),sizeof(int));
		memcpy(buffYama+2*sizeof(char)+sizeof(int),&(answerToYama->nodo),sizeof(int));

		if(answer->result == 'O'){
			log_info(logger,"bloque %d del nodo %d transformado",datos->blocks[i].pos,datos->node);
			send(masterSocket,buffYama,2*sizeof(char)+2*sizeof(int),0);
		}else{
			log_info(logger,"bloque %d del nodo %d no se pudo transformar",datos->blocks[i].pos,datos->node);
			//VER COMO MANEJAR ESTE ERROR EN YAMA
		}
	}
	log_trace(logger,"Nodo %d: Transformación finalizada", datos->node);
	free(answer);
/*
	for (i = 0; i < (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
*/
	free(buffer);
	free(nodeData->file);
	free(nodeData);
	free(scriptString);
	close(nodeSockets[datos->node]);
	return NULL;
}

//==============MAIN===========================================//

int transformFile(/*tr_datos yamaAnswer[], int totalRecords,*/ metrics *masterMetrics, char* filename){
	struct timeval tr_start,tr_end;
	gettimeofday(&tr_start,NULL);
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_TR *dataThreads = NULL;				//creo array de params para el hilo
	transform_rs* yamaAnswer;
	int totalRecords;

	yamaAnswer=sendTRequest(filename);
	totalRecords = yamaAnswer->bocksQuantity;

	int sizeBlocks = sizeof(tr_datos)*totalRecords;

	tr_datos* yamaBlocks = malloc(sizeBlocks);
	memcpy(yamaBlocks, yamaAnswer->blockData, sizeBlocks);

	log_info(logger, "Transformación iniciada");

	//TOMAR LA RESPUESTA DE YAMA

	while(recordCounter<totalRecords){
		nodo = yamaBlocks[recordCounter].nodo;	//init first key
		while(yamaBlocks[recordCounter].nodo==nodo && recordCounter<totalRecords){
			blocks=(block *) realloc(blocks,(sizeof(block)*(blockCounter+1)));
			blocks[blockCounter].pos = yamaBlocks[recordCounter].bloque;
			blocks[blockCounter].size = yamaBlocks[recordCounter].tamanio;
			strcpy(blocks[blockCounter].tmp,yamaBlocks[recordCounter].tr_tmp);
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
		strcpy(dataThreads[nodeCounter].ip,yamaBlocks[recordCounter-1].ip);
		dataThreads[nodeCounter].port=yamaBlocks[recordCounter-1].port;
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

	log_info(logger, "Tranformación finalizada");

	free(yamaAnswer);
	return EXIT_SUCCESS;
}
