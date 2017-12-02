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

void initPids(){
	int index = 0;
	for(index=0; index < 10; index++){
		pids[index] = 0;
	}
}

void killWorkers(int t){
	int index=0;
	for(index=0; index < 10; index++){
		if( pids[index] != 0)
			kill(pids[index], SIGKILL);
	}


	log_warning(logger,"Proceso Worker finalizado por señal");

	exit(1);

}

int main(){

	signal(SIGKILL,killWorkers);
	signal(SIGTERM,killWorkers);

	createLoggers();
	loadConfigs();
	map_data_node();
	//generateTempsFolder();
	loadServer();
	//-------------
	log_destroy(logger);
	config_destroy(config);
	return EXIT_SUCCESS;
}
