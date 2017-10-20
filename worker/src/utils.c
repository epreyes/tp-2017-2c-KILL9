/*
 * utils.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/utils.h"

void generateTempsFolder(){
	system("mkdir -p tmp");
};

void regenerateScript(char fileContent[11], FILE* script, char* scriptName){
	FILE* test = fopen("banana","w"); //ver si es necesario que le pase el nombre por el .sh en el socket
	if(test == NULL){
		log_error(logger,"Error al guardar el script");
		//abortar y comunicar a Master
	}
	fputs("hola mundo",test);
	fclose(test);
};


void loadConfigs(){
	char* CONFIG_PATH = "../properties/worker.properties";
	config = config_create(CONFIG_PATH);
	if(!(config_has_property(config,"TMP_FOLDER"))|| !(config_has_property(config,"WORKER_PUERTO"))){
		printf("\x1b[31m" "Error en archivo de configuraciones\n" "\x1b[0m");
		config_destroy(config);
		exit(0);
	}
}


void createLoggers(){
	char* LOG_PATH = "../logs/worker.log";
	logger = log_create(LOG_PATH,"worker",1,LOG_LEVEL_INFO);
}
