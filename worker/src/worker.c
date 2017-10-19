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
#include "headers/utils.h"
#include "headers/connectionsManager.h"
#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStore.h"


int main(){
	loadConfigs();
	createLoggers();
	loadServer();
	//-------------
	generateTempsFolder();


	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}
