/*
 * transform.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/transform.h"

//=============YAMA_REQUEST=============================//
transform_rs* sendTRequest(char* fileName){

if(replanification=='F'){
//PREPARO PAQUETE
	transform_rq* data;
	data = malloc(sizeof(transform_rq));
	data->code='T';
	data->fileNameSize= strlen(fileName)+1;
	data->fileName = malloc(data->fileNameSize);
	strcpy(data->fileName,fileName);

//SERIALIZO
	int bufferSize = sizeof(char)+sizeof(int)+(data->fileNameSize);
	void* buffer = malloc(bufferSize);
	memcpy(buffer,&(data->code),1);
	memcpy(buffer+sizeof(char),&(data->fileNameSize),sizeof(int));
	memcpy(buffer+sizeof(char)+sizeof(int),(data->fileName),(data->fileNameSize));
	free(data->fileName);
	free(data);

//ENVIO
	log_info(logger, "Solicitando datos de transformación a YAMA");
	if(send(masterSocket,buffer,bufferSize,0)<0){
		log_error(logger,"No se pudo conectar a YAMA");
		abortJob = 'T';
		free(buffer);
		return NULL;
	};
	free(buffer);
}
replanification = 'F';
//ESPERANDO RESPUESTA DE YAMA
	int i=0;
	char code;

	readBuffer(masterSocket, sizeof(char), &(code));
	if(code!='T'){ //'E':error
		log_warning(logger,"El archivo solicitado no existe o yamafs se encuentra desconectado.");
		abort();
	}
	transform_rs* yamaAnswer=malloc(sizeof(transform_rs));
	readBuffer(masterSocket, sizeof(int), &(yamaAnswer->bocksQuantity));
	yamaAnswer->blockData = malloc(sizeof(tr_datos)*(yamaAnswer->bocksQuantity));

	for(i=0;i<(yamaAnswer->bocksQuantity);++i){
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].nodo));
		readBuffer(masterSocket, 16, &(yamaAnswer->blockData[i].ip));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].port));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].bloque));
		readBuffer(masterSocket, sizeof(int), &(yamaAnswer->blockData[i].tamanio));
		readBuffer(masterSocket, 28, &(yamaAnswer->blockData[i].tr_tmp));
		printf("\tnodo:%d\tpos:%d\ttam:%d\tip:%s\t:port:%d\n", yamaAnswer->blockData[i].nodo, yamaAnswer->blockData[i].bloque, yamaAnswer->blockData[i].tamanio,yamaAnswer->blockData[i].ip,yamaAnswer->blockData[i].port);
	}
	return yamaAnswer;
}

//=============CHECK_REPLANIFICATION===========================//

void checkReplanification(int node){
	char code;
	sendErrorToYama('T',node);
	increaseMetricsError(&(masterMetrics.transformation.errors));

	log_warning(logger, "Reportado a YAMA, a la espera de replanificación...");
	readBuffer(masterSocket,sizeof(char),&code);
	if(code!='R'){
		log_error(logger, "Nodo %d: No es posible replanificar, se aborta la tarea", node);
		abortJob = 'T';
		increaseMetricsError(&(masterMetrics.transformation.errors));
	}else{
		replanification = 'T';
		log_trace(logger, "Nodo %d: Replanificado, reanudando transformación...", node);
	};
};

//=============THREAD_ACTION===================================//

void *runTransformThread(void* data){
	dataThread_TR* datos = (dataThread_TR*) data;
	int i, counter=0;
	char* scriptString;

	//CONEXION CON WORKER
		log_info(logger,"Estableciendo conexión con nodo %d",datos->node);
		if(openNodeConnection(datos->node, datos->ip, datos->port)!=0){
			checkReplanification(datos->node);
			return NULL;
		};


//PREPARO PAQUETE PARA WORKER---
	scriptString=serializeFile(script_transform);	//hacer que se genere una sola vez(?)
	tr_node* nodeData=malloc(sizeof(tr_node));
	nodeData->code='T';
	nodeData->fileSize=strlen(scriptString)+1;
	nodeData->file=malloc(nodeData->fileSize);		//ver el +1
	strcpy(nodeData->file,scriptString);
	nodeData->blocksSize=datos->blocksCount;
	free(scriptString);

//SERIALIZO---
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

	free(nodeData->file);
	free(nodeData);


//ENVIO DATOS A WORKER
	counter=1+4+(nodeData->fileSize)+4+(36*(datos->blocksCount));
	if(send(nodeSockets[datos->node],buffer,counter,0)<0){
		log_warning(logger, "Nodo %d: Desconectado", datos->node);
		checkReplanification(datos->node);
		free(buffer);
		return NULL;
	};
	free(buffer);
	log_trace(logger,"Nodo %d: Transformación iniciada", datos->node);

//METRICS
	gettimeofday(&(datos->metrics.start),NULL);

	pthread_mutex_lock(&parallelTasks);
		parallelAux+=datos->blocksCount;
		if(masterMetrics.transformation.parallelTask < parallelAux) masterMetrics.transformation.parallelTask= parallelAux;
	pthread_mutex_unlock(&parallelTasks);

//OBTENGO RESPUESTA DEL WORKER
	tr_node_rs* answer = malloc(sizeof(tr_node_rs));

	for(i=0;i < (datos->blocksCount); ++i){
		pthread_mutex_lock(&parallelTasks);
			parallelAux--;
		pthread_mutex_unlock(&parallelTasks);

		//Agrego recepción de error si se mata el worker
		if(readBuffer(nodeSockets[datos->node], sizeof(int), &(answer->block))!=0){
			checkReplanification(datos->node);
			return NULL;
		};
		readBuffer(nodeSockets[datos->node], sizeof(char), &(answer->result));

//RESPONDO A YAMA
		if(answer->result == 'O'){
			log_info(logger,"bloque %d del nodo %d transformado",datos->blocks[i].pos,datos->node);
			sendOkToYama('T',answer->block,datos->node);
		}else{
			log_warning(logger,"bloque %d del nodo %d no se pudo transformar",datos->blocks[i].pos,datos->node);
			checkReplanification(datos->node);
			free(answer);
			return NULL;
		}
	}
	free(answer);

	gettimeofday(&(datos->metrics.end),NULL);
	pthread_mutex_lock(&runTime);
	masterMetrics.transformation.runTime+= timediff(&(datos->metrics.end),&(datos->metrics.start));
	pthread_mutex_unlock(&runTime);
	log_trace(logger,"Nodo %d: Transformación finalizada (%.6gms)", datos->node, timediff(&(datos->metrics.end),&(datos->metrics.start)));
/*
	for (i = 0; i < (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
*/
	//free(datos);

	return NULL;
}

//==============MAIN===========================================//

int transformFile(char* filename){
	parallelAux = 0;
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_TR *dataThreads = NULL;				//creo array de params para el hilo
	transform_rs* yamaAnswer;
	int totalRecords;

	yamaAnswer=sendTRequest(filename);

	if(!yamaAnswer){
		log_error(logger, "TRANSFORMACIÓN ABORTADA");
		return EXIT_FAILURE;
	};

	totalRecords = yamaAnswer->bocksQuantity;

	int sizeBlocks = sizeof(tr_datos)*totalRecords;

	tr_datos* yamaBlocks = malloc(sizeBlocks);
	memcpy(yamaBlocks, yamaAnswer->blockData, sizeBlocks);

	log_info(logger, "Datos de yama obtenidos. %d bloques a transformar", totalRecords);
	log_info(logger, "Iniciando transformación...");


	while(recordCounter<totalRecords){
		nodo = yamaBlocks[recordCounter].nodo;	//init first key
		while(recordCounter<totalRecords && yamaBlocks[recordCounter].nodo==nodo){
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

//CREO LOS HILOS
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_create(&threads[threadIndex],NULL,(void*) runTransformThread, (void*) &dataThreads[threadIndex]);

//JOINEO HILOS

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex)
		pthread_join(threads[threadIndex],NULL);

//MÉTRICAS
	masterMetrics.transformation.tasks = totalRecords;
	masterMetrics.transformation.runTime = (masterMetrics.transformation.runTime)/totalRecords;

//LIBERO MEMORIA----------------
	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].blocks);

	free(dataThreads);
	free(threads);
	free(blocks);
	free(yamaAnswer->blockData);
	free(yamaAnswer);
	free(yamaBlocks);


//EJECUTO REPLANIFICACIÓN-------S-
	if(replanification == 'T'){
		log_info(logger,"Replanificación confirmada, iniciando transformación de pendientes");
		transformFile(filename);
	}else{
		if(abortJob=='0'){
			log_trace(logger, "TRANSFORMACIÓN FINALIZADA");
			return EXIT_SUCCESS;
		}else{
			log_error(logger, "TRANSFORMACIÓN ABORTADA");
			return EXIT_FAILURE;
		}
	}
}
