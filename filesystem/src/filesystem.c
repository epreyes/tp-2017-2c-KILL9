/*
 ============================================================================
 Name        : filesystem.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <connections/socket.h>
#include <stdio.h>
#include <stdlib.h>
int main(void) {

	//Prueba socket servidor
	int fd_client_socket;
	int listenning_socket = open_socket(5003,10);

	for(;;){
		fd_client_socket = accept_connection(listenning_socket);
		puts("se coneceto client");
	}


	return EXIT_SUCCESS;
}
