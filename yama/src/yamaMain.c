/*
 ============================================================================
 Name        : yamaMain.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "yama.h"

int main(void) {
	Yama yama = configYama();
	yama.yama_server = startServer(config_get_int_value(yama.config, "YAMA_PUERTO"));

	waitForMasters(&yama);


	return EXIT_SUCCESS;
}
