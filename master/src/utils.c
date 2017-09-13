/*
 * utils.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/utils.h"


double timediff(struct timeval *a, struct timeval *b){
	return ((double)(a->tv_sec + (double)a->tv_usec/1000000)-(double)(b->tv_sec + (double)b->tv_usec/1000000))*1000.0;
}

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

void validateConfigs(){
	char* CONFIG_PATH = "../properties/master.properties";
	t_config* config;
	config = config_create(CONFIG_PATH);
	if(!(config_has_property(config,"YAMA_IP"))|| !(config_has_property(config,"YAMA_PUERTO"))){
		printf("\x1b[31m" "Error en archivo de configuraciones\n" "\x1b[0m");
		config_destroy(config);
		exit(0);
	}
	config_destroy(config);
}
