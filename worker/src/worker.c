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
	loadServer();
	//-------------
	//openFileSystemConnection();
	generateTempsFolder();
	readMasterBuffer();
	printf("\nREDUCCION LOCAL\n");
	readMasterBuffer();
	//-------------
	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}
