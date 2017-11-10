/*
 ============================================================================
 Name        : yamaMain.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "headers/connectionManager.h"
#include "headers/yama.h"
#include "headers/signalManager.h"

int main(void) {

	/*capturo la señal SIGUSR1*/
	signal(SIGUSR1,reloadConfig);

	/* inicializo yama */
	init();
	 printf("\n\nYAMA process ID : %d\n\n\n", getpid());
	/* espero las conexiones de los masters */
	waitMastersConnections();

	//getFileSystemInfo("/tmp/archivoooo-teststs.bin");

	return EXIT_SUCCESS;
}
