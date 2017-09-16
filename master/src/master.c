/*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "answers.h"

typedef struct block{
	int pos;
	int size;
}block;

typedef struct{
	int		node;
	char* 	conector;
	block* 	blocks;
	int		blocksCount;
}dataThread;

void validateArgs(int argc, char* argv[]){
	int count;
	if(argc!=5){
			printf("\x1b[31m" "> Número de argumentos inválido! \n" "\x1b[0m");
			exit(0);
	};
	for(count=3;count<=4;++count){
		if(strncmp(argv[count],"yamafs:/",8)!=0){
			printf("\x1b[31m" "El archivo %s debe pertenecer a yamafs, no olvide el prefijo yamafs:\n" "\x1b[0m",argv[count]);
			exit(0);
		}
	}
};

int runWorkerThread(void *args){
	int i;
	dataThread* datos;
	datos = malloc(sizeof(dataThread));
	datos = (dataThread*) args;
	usleep(1000);
	printf("hilo iniciado:%d\n servidor:%s\n",datos->node,datos->conector);
	for (i = 0; i <= (datos->blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos->node, datos->blocks[i].pos, datos->blocks[i].size);
	}
	return EXIT_SUCCESS;
}

/////////////////////////TRANSFORM///////////////////////////////////

int transformFile(tr_datos yamaAnswer[], int totalRecords){
	int blockCounter=0,recordCounter=0,threadCounter=0, threadIndex=0, nodo;
	block *blocks=NULL;
	blocks=malloc(sizeof(block));
	pthread_t *threads;
	threads=malloc(sizeof(pthread_t)*10); //pasar a realloc dinámico
	dataThread *dataThreads;
	dataThreads = malloc(sizeof(dataThread)*10); //pasar a realloc dinámico

	while(recordCounter<totalRecords){
		nodo = yamaAnswer[recordCounter].nodo;	//init first key
		while(yamaAnswer[recordCounter].nodo==nodo && recordCounter<=totalRecords){
			blocks=realloc(blocks,sizeof(block)*(blockCounter+1));
			blocks[blockCounter].pos = yamaAnswer[recordCounter].bloque;
			blocks[blockCounter].size = yamaAnswer[recordCounter].tamanio;
			blockCounter++;
			recordCounter++;
		};
		dataThreads[threadCounter].node=nodo;
		dataThreads[threadCounter].conector=yamaAnswer[recordCounter-1].direccion;
		dataThreads[threadCounter].blocks=blocks;
		dataThreads[threadCounter].blocksCount=blockCounter-1;
		int i=0;
		for(i=0;i<=blockCounter-1;++i){
			printf("\t thread: %d \t pos:%d\t size:%d\n",threadCounter, dataThreads[threadCounter].blocks[i].pos,dataThreads[threadCounter].blocks[i].size);
		}
		pthread_create(&threads[threadCounter],NULL,(void*) runWorkerThread,(void*) &dataThreads[threadCounter]);
		threadCounter++;
		blockCounter = 0;
	};
	for (threadIndex=0;threadIndex<=threadCounter;++threadIndex){
		pthread_join(threads[threadCounter],NULL);
	}
	usleep(100000);
	free(blocks);
	free(dataThreads);
	free(threads);
	return EXIT_SUCCESS;
};


///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){
	int answerSize = sizeof(tr_answer)/sizeof(tr_answer[0]);

	validateArgs(argc, argv);					//valido argumentos
	transformFile(tr_answer,answerSize);		//se conecta a YAMA y devuelve array de struct
	//runLocalReduction(argv[2]);				//ordena ejecución de Reductor Local
	//runGlobalReduction						//ordena ejecución de Reductor Global
	//saveResult(argv[])						//ordena guardado en FileSystem
	return EXIT_SUCCESS;
}
