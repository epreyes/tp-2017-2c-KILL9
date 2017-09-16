/*
 ============================================================================
 Name        : yama.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "yama.h"

int main(void) {
	Yama yama = configYama();
	yama.yama_server = startServer(yama.port);

	waitForMasters(&yama);


	return EXIT_SUCCESS;
}
