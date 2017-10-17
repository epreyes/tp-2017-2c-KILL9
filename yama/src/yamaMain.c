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

int main(void) {

	/* inicializo yama */
	init();

	/* espero las conexiones de los masters */
	waitMastersConnections();

	//getFileSystemInfo("/tmp/archivoooo-teststs.bin");

	return EXIT_SUCCESS;
}
