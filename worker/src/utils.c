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

char* regenerateScript(char* fileContent, FILE* script, char operation, int socket){
	char* scriptName = generateScriptName(operation, socket);
	char command[25];
	script = fopen(scriptName,"w");
	if(script == NULL){
		log_error(logger,"Error al guardar el script %s", scriptName);
		//abortar y comunicar a Master
	}
	fputs(fileContent,script);
	fclose(script);
	log_info(logger,"Script almacenado en \"%s\"", scriptName);

	//---doy permisos de ejecución---//
	strcpy(command,"chmod 555 ");				//pasar nombre de archivo a dinámico
	strcat(command, scriptName);

	//printf("%s",command);
	system(command);							//pasar nombre de archivo a dinámico
	log_info(logger,"Permisos de ejecución de %s concedidos", scriptName);
	return scriptName;
};


void loadConfigs(){
	char* CONFIG_PATH = "properties/worker.properties";
	config = config_create(CONFIG_PATH);
	if(!(config_has_property(config,"TMP_FOLDER"))|| !(config_has_property(config,"WORKER_PUERTO"))){
		log_error(logger, "Error al cargar archivos de configuración");
		config_destroy(config);
		exit(0);
	}
	log_info(logger, "Archivo de configuraciones cargado");
}


void createLoggers(){
	char* LOG_PATH = "../logs/worker.log";
	logger = log_create(LOG_PATH,"worker",1,LOG_LEVEL_TRACE);
	log_info(logger, "Logger generado");
}

long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);
    return te.tv_usec+te.tv_sec;
}

char* generateScriptName(char operation, int master){
	char* name;
	long timestamp = current_timestamp();
	asprintf(&name, "%c%d-%ld",operation,master,timestamp);
	return name;
}
