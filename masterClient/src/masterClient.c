/*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Master Process
 ============================================================================
 */

//=========LIBREARIES==========//

#include "headers/master.h"
#include "headers/metrics.h"
#include "headers/answers.h"

#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStorage.h"

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){

	createLoggers();
	//validateArgs(argc, argv);
	loadConfigs();
	openWorkerConnection();


	char code='L';
	send(masterSocket,&code,sizeof(char),0);

	return EXIT_SUCCESS;
};
