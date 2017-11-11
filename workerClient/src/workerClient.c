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


///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){

	createLoggers();
	loadConfigs();
	openWorkerConnection();


	char code='R';
	send(masterSocket,&code,sizeof(char),0);

	return EXIT_SUCCESS;
};
