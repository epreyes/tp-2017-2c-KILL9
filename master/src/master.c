/*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Master Process
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "answers.h"
#include "master.h"

typedef struct block{
	int pos;
	int size;
}block;

typedef struct dataThread{
	int		node;
	char 	conector[100];
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

void *runWorkerThread(void* data){
	int i;
	dataThread* datos = (dataThread*) data;
	printf("hilo iniciado:%d\n servidor:%s\n",datos[0].node,datos[0].conector);
	for (i = 0; i <= (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
	//conectarse con WORKER y esperar respuesta.
	return NULL;
}

////////////////////////////////////TRANSFORM/////////////////////////////////////////

int transformFile(tr_datos yamaAnswer[], int totalRecords){
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 					//creo array de hilos dinámico
	dataThread *dataThreads = NULL;				//creo array de params para el hilo

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
		dataThreads=(dataThread *) realloc (dataThreads,(sizeof(dataThread)*(nodeCounter+1)));
		//defino los parámetros
		//copio los bloques del nodo para no perderlos
		dataThreads[nodeCounter].blocks=(block *) malloc(sizeof(block)*(blockCounter));
		dataThreads[nodeCounter].blocks=(block *) memcpy(dataThreads[nodeCounter].blocks,blocks, sizeof(block)*(blockCounter));

		dataThreads[nodeCounter].node=nodo;
		strcpy(dataThreads[nodeCounter].conector,yamaAnswer[recordCounter-1].direccion);
		dataThreads[nodeCounter].blocksCount=blockCounter-1;
		nodeCounter++;
		blockCounter = 0;
	}

//---creo los hilos------------
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		pthread_create(&threads[threadIndex],NULL,(void*) runWorkerThread, (void*) &dataThreads[threadIndex]);
	}
//---joineo los hilos----

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		printf("Espero al thread %d\n", threadIndex);
		pthread_join(threads[threadIndex],NULL);
	}
/*
	*/
//------libero memoria-----------------
	int var=0;
	for (var = 0; var < nodeCounter; ++var) {
		free(dataThreads[var].blocks);
	}

	free(blocks);
	free(dataThreads);
	free(threads);

	return EXIT_SUCCESS;
}

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){
	//recibo respuesta de Yama
	int answerSize = sizeof(tr_answer)/sizeof(tr_answer[0]);
	validateArgs(argc, argv);					//valido argumentos
	transformFile(tr_answer,answerSize);		//se conecta a YAMA y devuelve array de struct
	//runLocalReduction(argv[2]);				//ordena ejecución de Reductor Local
	//runGlobalReduction						//ordena ejecución de Reductor Global
	//saveResult(argv[])						//ordena guardado en FileSystem
	return EXIT_SUCCESS;
};
