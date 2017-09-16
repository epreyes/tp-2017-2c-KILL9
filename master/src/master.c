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

int runWorkerThread(int node, block* blocks, int quantity, char* server){
	int i;
	printf("estableciendo conexion con nodo %d del server: %s \n", node, server);
	usleep(1000000);
	for (i = 0; i <= quantity; ++i) {
		printf("\t posicion: %d\n", blocks[i].pos);
		printf("\t tamanio: %d\n", blocks[i].size);
	};
	return EXIT_SUCCESS;
}


void holaHilo(void *args){
	int i;
	dataThread* datos;
	datos = malloc(sizeof(dataThread));
	datos = (dataThread*) args;
	printf("hilo iniciado:%d\n",datos->node);
	printf("servidor:%s\n",datos->conector);
	printf("bloques:\n");
	for (i = 0; i <= (datos->blocksCount); ++i){
		printf("\t posicion: %d\n", datos->blocks[i].pos);
		printf("\t tamanio: %d\n", datos->blocks[i].size);
	}
}

/////////////////////////TRANSFORM///////////////////////////////////

int transformFile(tr_datos yamaAnswer[], int totalRecords){
	int blockCounter=0,recordCounter=0, nodo;
	block *blocks=NULL;
	blocks=malloc(sizeof(block));
	pthread_t thread;
	dataThread *dataThread1;
	dataThread1 = malloc(sizeof(dataThread));

	while(recordCounter<totalRecords){
		nodo = yamaAnswer[recordCounter].nodo;	//init first key
		while(yamaAnswer[recordCounter].nodo==nodo && recordCounter<=totalRecords){
			blocks=realloc(blocks,sizeof(block)*(blockCounter+1));
			blocks[blockCounter].pos = yamaAnswer[recordCounter].bloque;
			blocks[blockCounter].size = yamaAnswer[recordCounter].tamanio;
			blockCounter++;
			recordCounter++;
		};
		dataThread1->node=nodo;
		dataThread1->conector=yamaAnswer[recordCounter-1].direccion;
		dataThread1->blocks=blocks;
		dataThread1->blocksCount=blockCounter-1;

		pthread_create(&thread,NULL,holaHilo,(void*) dataThread1);
		pthread_join(thread,NULL);
		printf("se creo el hilo %d", nodo);
		//runWorkerThread(nodo, blocks, blockCounter-1, yamaAnswer[nodo].direccion); //crea hilo, se conecta, pasa datos y espera respuesta
		blockCounter = 0;
	};
	usleep(1000000);
	free(blocks);
	free(dataThread1);
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
