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

int ejecutarWorker(){
	usleep(1000);
	printf("worker ejecutado");
	return EXIT_SUCCESS;;
}
/*
int ejecutarHilos() {
	pthread_t threads[3];
	void * retvals[3];
	int count,i;
	for(count = 0; count < 3; ++count){
		if(pthread_create(&threads[count],NULL,(void*)ejecutarWorker,NULL)!=0){
			printf("error: no se pudo iniciar el nodo %d\n", count);
			break;
		};
		printf("worker %d ejecutando\n", count);
	};
	printf("Ejecutando ");
	for(i = 0; i < count; ++i){
		if(pthread_join(threads[i], &retvals[i])!=0){
			printf("error: no se pudo joinear el hilo del nodo %d\n", count);
		}else
			printf("Hilo del nodo %d joineado con exito \n", i);
	}
	puts("procesamiento de workers finalizado...");
	return EXIT_SUCCESS;
}
*/
void getFileInfo(char* fileName){
	printf("hola soy %s \n",fileName);
}

void validateArgs(int argc, char* argv[]){
	int i;
	if(argc!=5){
			printf("\x1b[31m" "> Número de argumentos inválido! \n" "\x1b[0m");
			exit(0);
	};
	for(i=3;i<=4;++i){
		if(strncmp(argv[i],"yamafs:/",8)!=0){
			printf("\x1b[31m" "El archivo %s debe pertenecer a yamafs, no olvide el prefijo yamafs:\n" "\x1b[0m",argv[i]);
			exit(0);
		}
	}
};

int prepararConexion(tr_datos respuesta[]){
	int i=0;
	int j;
	int nodo, cantRegistros;
	int cantNodos;
	struct bloque{
		int posicion;
		int tamanio;
	};;
	struct bloque bloques[256];
	//-----------------
		pthread_t *threads;
		void * retvals;

	//-----------------

	cantRegistros=sizeof(respuesta);

	while(i<=cantRegistros){
		cantNodos=0;
		nodo = respuesta[i].nodo;
		j=0;
		while(respuesta[i].nodo==nodo && i<=cantRegistros){
			bloques[j].posicion = respuesta[i].bloque;
			bloques[j].tamanio = respuesta[i].tamanio;
			i++; j++;
		};
		//Inicio Hilo
		if(pthread_create(&threads[cantNodos],NULL,(void*)ejecutarWorker,NULL)!=0){
			printf("error: no se pudo iniciar el nodo %d\n",cantNodos);
			break;
		};
		cantNodos++;
	};

	for(i = 0; i < cantNodos; ++i){
		if(pthread_join(threads[i], &retvals[i])!=0){
			printf("error: no se pudo joinear el hilo del nodo %d\n", cantNodos);
		}else
			printf("Hilo del nodo %d joineado con exito \n", i);
	}
	return EXIT_SUCCESS;
};

void transformar(char* program){
	//solicitarDatos(); 				//conecta por socket y obtiene los datos de YAMA
	prepararConexion(tr_answer);		//ordena los datos para pedir transform
	//ejecutarHilos();
}

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){
	//validateArgs(argc, argv);					//valido argumentos
	transformar(argv[1]);					//se conecta a YAMA y devuelve array de struct
	//runLocalReduction(argv[2]);				//ordena ejecución de Reductor Local
	//runGlobalReduction						//ordena ejecución de Reductor Global
	//saveResult(argv[])						//ordena guardado en FileSystem
	return EXIT_SUCCESS;
}

