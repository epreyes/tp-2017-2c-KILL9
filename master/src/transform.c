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
	log_info(logger, "Solicitando datos de transformación a YAMA");
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
	openNodeConnection(datos[0].node, datos[0].ip, datos[0].port);
	counter=1+4+(nodeData->fileSize)+4+(36*(datos->blocksCount));

//--Envío---
	send(nodeSockets[datos->node],buffer,counter,0);
	log_trace(logger,"Nodo %d: Transformación iniciada", datos->node);
//--Espero respuesta

	tr_node_rs* answer = malloc(sizeof(tr_node_rs));

	void readBuffer(int socket,int size,void* destiny){
		void* buffer = malloc(size);
		int bytesReaded = recv(socket, buffer, size, MSG_WAITALL);
		if (bytesReaded <= 0) {
			log_warning(logger,"Nodo %d: desconectado",socket);
			exit(1);
		}
		memcpy(destiny, buffer, size);
		free(buffer);
	}

	for(i=0;i < (datos->blocksCount); ++i){
		//-----
		readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->block));
			//printf("BLOQUE:%d\n",answer->block);
		readBuffer(nodeSockets[datos->node], sizeof(char), &(answer->result));
			//printf("RESULT:%c\n",answer->result);
		readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->runtime));
			//printf("METRIC:%d\n",answer->runtime);
		if(answer->result == 'O'){
			log_info(logger,"bloque %d del nodo %d transformado",datos->blocks[i].pos,datos->node);
		}else{
			log_info(logger,"bloque %d del nodo %d no se pudo transformar",datos->blocks[i].pos,datos->node);
		}
			//ENVIAR A YAMA
		//send(masterSocket,buffer,5+(data->fileNameSize),0);
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

int transformFile(tr_datos yamaAnswer[], int totalRecords, metrics *masterMetrics, char* filename){
	struct timeval tr_start,tr_end;
	gettimeofday(&tr_start,NULL);
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_TR *dataThreads = NULL;				//creo array de params para el hilo

	sendTRequest(filename);

	log_info(logger, "Transformación iniciada");

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
		strcpy(dataThreads[nodeCounter].ip,yamaAnswer[recordCounter-1].ip);
		dataThreads[nodeCounter].port=yamaAnswer[recordCounter-1].port;
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

	return EXIT_SUCCESS;
}
