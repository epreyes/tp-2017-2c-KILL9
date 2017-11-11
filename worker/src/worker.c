/*
 ============================================================================
 Name        : Worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "headers/worker.h"
#include "headers/connectionsManager.h"


int main(){
	createLoggers();
	loadConfigs();
	map_data_node();

	loadServer();
	//-------------
	//openFileSystemConnection();
	//generateTempsFolder();
	//while(1){
	//Atiendo conexiones 

	//readMasterBuffer();
	startWorkerServer();

	//}
	//-------------
	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}
