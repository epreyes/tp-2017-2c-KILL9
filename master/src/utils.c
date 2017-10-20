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
	char* CONFIG_PATH = "../properties/master.properties";
	config = config_create(CONFIG_PATH);
	if(!(config_has_property(config,"YAMA_IP"))|| !(config_has_property(config,"YAMA_PUERTO"))){
		log_error(logger,"error en el archivo de configuración");
		config_destroy(config);
		exit(0);
	}
}


void createLoggers(){
	char* LOG_PATH = "../logs/master.log";
	logger = log_create(LOG_PATH,"master",1,LOG_LEVEL_INFO);
}
