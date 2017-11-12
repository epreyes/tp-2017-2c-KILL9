/*
 * utils.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/utils.h"

//calculo el tiempo entre 2 timeval
double timediff(struct timeval *a, struct timeval *b){
	return ((double)(a->tv_sec + (double)a->tv_usec/1000000)-(double)(b->tv_sec + (double)b->tv_usec/1000000))*1000.0;
}


void validateArgs(int argc, char* argv[]){
	int count;
	if(argc!=5){
			log_error(logger,"número de argumentos inválido");
			exit(0);
	};
	for(count=3;count<=4;++count){
		if(strncmp(argv[count],"yamafs:/",8)!=0){
			log_error(logger, "El archivo %s debe pertenecer a yamafs", argv[count]);
			exit(0);
		}
	}
};

char* serializeFile(FILE* file){
	int caracter, fileSize, charCounter=0;
	char* stringFile;

//---obtengo el tamaño del archivo

	fseek(file,0,SEEK_END);
	fileSize = ftell(file)+1;
	stringFile=malloc(fileSize);
	rewind(file);

//---paso contenido a string

	while((caracter = fgetc(file))!= EOF){
		stringFile[charCounter] = caracter;
		charCounter++;
	}
	stringFile[charCounter] = '\0';
	return stringFile;
}

void loadScripts(char* transformScript, char* reductionScript){
	script_transform=fopen(transformScript,"r");
	script_reduction=fopen(reductionScript,"r");
//---valído existencia de los archivos
	if(script_transform==NULL || script_reduction==NULL){
		log_error(logger,"no existe el archivo de transformación solicitado");
		exit(1);
	}
	if(script_transform==NULL || script_reduction==NULL){
		log_error(logger,"no existe el archivo de reduccion solicitado");
		exit(1);
	}
}

//------------------------------

void loadConfigs(){
	char* CONFIG_PATH = "properties/master.properties";
	config = config_create(CONFIG_PATH);
	if(!(config_has_property(config,"YAMA_IP"))|| !(config_has_property(config,"YAMA_PUERTO"))){
		log_error(logger,"error en el archivo de configuración");
		config_destroy(config);
		exit(0);
	}
}


void createLoggers(){
	char* LOG_PATH = "logs/master.log";
	logger = log_create(LOG_PATH,"master",1,LOG_LEVEL_TRACE);
}

void readBuffer(int socket,int size,void* destiny){
	void* buffer = malloc(size);
	int bytesReaded = recv(socket, buffer, size, MSG_WAITALL);
	if (bytesReaded <= 0){
		log_warning(logger,"Socket %d: desconectado",socket);
		exit(1);
	}
	memcpy(destiny, buffer, size);
	free(buffer);
}

int sendOkToYama(char opCode, int block, int node){
	ok* message = malloc(sizeof(ok));

	int bufferSize = sizeof(char)*2+sizeof(int)*2;
	void* buffer = malloc(bufferSize);

	message->code='O';
	message->opCode=opCode;
	message->bloque=block;
	message->nodo=node;

	memcpy(buffer,&(message->code),sizeof(char));
	memcpy(buffer+sizeof(char),&(message->opCode),sizeof(char));
	memcpy(buffer+2*sizeof(char),&(message->bloque),sizeof(int));
	memcpy(buffer+2*sizeof(char)+sizeof(int),&(message->nodo),sizeof(int));

	//printf("\tcode:%c \topCode:%c \tbloque:%d \tnodo:%d\n",answerToYama->code,answerToYama->opCode,answerToYama->bloque,answerToYama->nodo);

	free(message);

	if(send(masterSocket,buffer,bufferSize,0)<0){
		free(buffer);
		log_warning(logger,"YAMA está desconectado");
		return 1;
	}else{
		free(buffer);
		return 0;
	}
};

int sendErrorToYama(char opCode, int node){
	error_rq* error = malloc(sizeof(error_rq));
	int bufferSize = sizeof(char)*2+sizeof(int);

	error->code = 'E';
	error->opCode = opCode;
	error->nodo = node;

	void* buff = malloc(bufferSize);
	memcpy(buff,&(error->code), sizeof(char));
	memcpy(buff+sizeof(char),&(error->opCode), sizeof(char));
	memcpy(buff+sizeof(char)*2, &(error->nodo), sizeof(int));
	free(error);

	if(send(masterSocket,buff,bufferSize,0)<0){
		log_warning(logger,"YAMA está desconectado");
		free(buff);
		return 1;
	}else{
		free(buff);
		return 0;
	}
};

void increaseMetricsError(int* metric){
	pthread_mutex_lock(&errors);
	metric[0]++;
	pthread_mutex_unlock(&errors);
};
