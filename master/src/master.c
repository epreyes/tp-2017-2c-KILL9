/*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Master Process
 ============================================================================
 */

//=====LIBREARIES=========//

#include "master.h"

//=====STRUCTURES=========//


typedef struct block{
	int pos;
	int size;
}block;

typedef struct dataThread_TR{
	int		node;
	char 	conector[21];
	block* 	blocks;
	int		blocksCount;
}dataThread_TR;


typedef char tr_tmp[28];

typedef struct dataThread_LR{
	int		node;
	char 	conector[21];
	tr_tmp* tr_tmps;
	char	rl_tmp[28];
	int 	tmpsCounter;
}dataThread_LR;

typedef struct dataNodes{
	int		node;
	char 	conector[21];
	char	rl_tmp[28];
}dataNodes;

typedef struct dataThread_GR{
	int			leadNode;
	char 		leadConector[21];
	char		rg_tmp[24];
	dataNodes* 	brothersData;
	int			brothersCount;
}dataThread_GR;


//=============================================

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

////////////////////////////////////TRANSFORM/////////////////////////////////////////

void *runTransformThread(void* data){
	int i;
	dataThread_TR* datos = (dataThread_TR*) data;
	printf("hilo iniciado:%d\n servidor:%s\n",datos[0].node,datos[0].conector);
	for (i = 0; i <= (datos[0].blocksCount); ++i){
		printf("\t nodo:%d \t pos:%d  \t tam:%d\n", datos[0].node, datos[0].blocks[i].pos, datos[0].blocks[i].size);
	}
	//conectarse con WORKER y esperar respuesta.
	return NULL;
}

int transformFile(tr_datos yamaAnswer[], int totalRecords){
	int blockCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	block* blocks = NULL;
	pthread_t *threads = NULL; 					//creo array de hilos dinámico
	dataThread_TR *dataThreads = NULL;				//creo array de params para el hilo

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
		pthread_create(&threads[threadIndex],NULL,(void*) runTransformThread, (void*) &dataThreads[threadIndex]);
	}
//---joineo los hilos----

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		pthread_join(threads[threadIndex],NULL);
	}
/*
	*/
//------libero memoria-----------------
	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].blocks);

	free(blocks);
	free(dataThreads);
	free(threads);

	return EXIT_SUCCESS;
}

////////////////////////////////////LOCAL_REDUCTION/////////////////////////////////////////
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


int runLocalReduction(rl_datos yamaAnswer[], int totalRecords){
	int tmpsCounter=0,recordCounter=0, nodeCounter=0, threadIndex=0, nodo;
	tr_tmp* tr_tmps = NULL;
	pthread_t *threads = NULL; 						//creo array de hilos dinámico
	dataThread_LR *dataThreads = NULL;				//creo array de params para el hilo

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
	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		pthread_create(&threads[threadIndex],NULL,(void*) runLocalRedThread, (void*) &dataThreads[threadIndex]);
	}
//------joineo los hilos----

	for(threadIndex=0;threadIndex<nodeCounter;++threadIndex){
		pthread_join(threads[threadIndex],NULL);
	}
//------libero memoria-----------------

	int var=0;
	for (var = 0; var < nodeCounter; ++var)
		free(dataThreads[var].tr_tmps);

	free(tr_tmps);
	free(dataThreads);
	free(threads);

	return EXIT_SUCCESS;
}


////////////////////////////////////GLOBAL_REDUCTION/////////////////////////////////////////

int runGlobalReduction(rg_datos yamaAnswer[], int totalRecords){
	int recordCounter=0;
	dataNodes *brothersData = NULL;
	brothersData = (dataNodes*) malloc(sizeof(dataNodes));
	dataThread_GR* dataThread = NULL;
	dataThread = (dataThread_GR*) malloc(sizeof(dataThread_GR));
	//recorro los registros de la respuesta
	while(recordCounter<totalRecords){
		if(yamaAnswer[recordCounter].encargado==1){
			dataThread->leadNode = yamaAnswer[recordCounter].nodo;
			strcpy(dataThread->leadConector, yamaAnswer[recordCounter].direccion);
			strcpy(dataThread->rg_tmp, yamaAnswer[recordCounter].rg_tmp);
		}else{
			brothersData = (dataNodes*)realloc(brothersData,(sizeof(dataNodes)*(recordCounter+1)));
			strcpy(brothersData[recordCounter].conector, yamaAnswer[recordCounter].direccion);
			brothersData[recordCounter].node = yamaAnswer[recordCounter].nodo;
			strcpy(brothersData[recordCounter].rl_tmp, yamaAnswer[recordCounter].rl_tmp);
		}
		recordCounter++;
	};
	dataThread->brothersData = (dataNodes *) malloc(sizeof(dataNodes)*recordCounter);
	dataThread->brothersData = (dataNodes *) memcpy(dataThread->brothersData,brothersData, sizeof(dataNodes)*recordCounter);
	dataThread->brothersCount = recordCounter;


	free(brothersData);
	free(dataThread->brothersData);
	free(dataThread);

	return EXIT_SUCCESS;
}

////////////////////////////////////SAVE_RESULTS/////////////////////////////////////////

int saveResult(rg_datos yamaAnswer[], int totalRecords){
	/*
	..

	//conectarseConWorker y pasar yamaAnswer.rg_tmp

	..
	*/
	return EXIT_SUCCESS;
}



///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){
	//recibo respuesta de Yama
	int answerSize_TR = sizeof(tr_answer)/sizeof(tr_answer[0]);
	int answerSize_RL = sizeof(rl_answer)/sizeof(rl_answer[0]);
	int answerSize_RG = sizeof(rg_answer)/sizeof(rg_answer[0]);
	int answerSize_AF = sizeof(af_answer)/sizeof(af_answer[0]);

	validateArgs(argc, argv);						//valido argumentos
	transformFile(tr_answer,answerSize_TR);			//se conecta a YAMA y devuelve array de struct
	runLocalReduction(rl_answer,answerSize_RL);		//ordena ejecución de Reductor Local
	runGlobalReduction(rg_answer,answerSize_RG);	//ordena ejecución de Reductor Global
	saveResult(af_answer,answerSize_AF);			//ordena guardado en FileSystem
	return EXIT_SUCCESS;
};
