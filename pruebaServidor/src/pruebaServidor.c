/*
 ============================================================================
 Name        : pruebaServidor.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "Socket.h"

#define HEAD_SIZE 8

int procesarPersona(int sock, void* buff, int tam) {
	recv(sock, buff, tam, 0);

	typedef struct estructura {
		char nombre[20];
		int dni;
	} estructura;

	estructura person;
	memcpy(person.nombre, buff, sizeof(person.nombre));
	memcpy(&(person.dni), buff + sizeof(person.nombre), sizeof(person.dni));

	printf("Recibi una persona llamada %s con dni %d\n", person.nombre,
			person.dni);

	return 1;
}

int procesarEmpresa(int sock, void* buff, int tam) {
	recv(sock, buff, tam, 0);

	typedef struct empre{
				char nombre[20];
				int cuit;
				int tele;
			}empre;

	empre empresa;
	memcpy(empresa.nombre, buff, sizeof(empresa.nombre));
	memcpy(&(empresa.cuit), buff + sizeof(empresa.nombre), sizeof(empresa.cuit));
	memcpy(&(empresa.tele), buff + sizeof(empresa.nombre)+sizeof(empresa.cuit), sizeof(empresa.cuit));

	printf("Recibi una EMPRESA llamada %s con cuit %d y telef %d\n", empresa.nombre,
			empresa.cuit, empresa.tele);

	return 1;
}

int procesarDatos(void* buff, int sock, int bytes) {
	typedef struct header {
		int tipo;
		int tam;
	} header;

	header head; //= *((header*)buff);
	memcpy(&head, buff, HEAD_SIZE);

	printf(
			"Recibi un head de tipo %d y tamanio %d\nAhora recibo el dato posta: \n",
			head.tipo, head.tam);

	switch (head.tipo) {
	case 1:
		procesarPersona(sock, buff, head.tam);
		break;
	case 2:
		procesarEmpresa(sock, buff, head.tam);
		break;
	}

	return 1;
}

int obtenerRespuesta(int socket, int* nbytes, fd_set* master) {

	void* buff;

	*nbytes = recv(socket, buff, HEAD_SIZE, 0);
	if (*nbytes <= 0) { //si hubo un error en la conexion puede ser cero o menos uno.

		// error o conexión cerrada por el cliente
		if (*nbytes == 0) {
			// conexión cerrada
			printf("se desconecto el socket %d \n", socket);
		} else {
			perror("recv");
		}

		close(socket); // bye!
		FD_CLR(socket, &(*master)); // eliminar del conjunto maestro
	}

	else { //si recibi datos
		   // tenemos datos de algún cliente
		printf("\nRecibi %d bytes\n", *nbytes);
		procesarDatos(buff, socket, *nbytes);
	}
	return *nbytes;
}

Client aceptarConexion(Server* servidor, fd_set* master, int hightSd) {
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);
	int client = accept(servidor->server_socket,
			(struct sockaddr *) &client_address, &addrlen);
	if (client == -1) {
		perror("accept");
	} else {
		//si no hubo error en la aceptacion, meto el nuevo socket en el listado master.
		FD_SET(client, &(*master)); // añadir al conjunto maestro
		if (client > hightSd) {    // actualizar el máximo
			servidor->higherSocketDesc = client;
		}
		printf("selectserver: new connection from %s on socket %d\n",
				inet_ntoa(client_address.sin_addr), client);
	}
	Client theClient;
	theClient.socket_id = client;
	theClient.address = client_address;

	return theClient;
}

void explorarConexiones(Server* servidor, fd_set* readfds, fd_set* master) {
	int i = 0;
	Client client;
	int nbytes = 0;
	int hightSd = servidor->higherSocketDesc;

	for (i = 0; i <= hightSd; i++) {

		if (FD_ISSET(i, &(*readfds))) { //si alguno de los sockets esta en readfds, quiere decir que algo cambio en ese socket.

			if (i == servidor->server_socket) { //si el socket que cambio es el servidor, es que hubo una nueva conexion. Por lo tanto tengo que aceptarla.
				client = aceptarConexion(servidor, &(*master), hightSd);
				printf("\nse me conecto el socket %d\n", client.socket_id);
			} else {
				//si el que cambio es un cliente, recibo los datos.
				// gestionar datos de un cliente
				printf("\n\nRecibi %d bytes de client %d!\n\n",
						obtenerRespuesta(i, &nbytes, &(*master)), i);
			} // Esto es ¡TAN FEO!
		}

	}
}

int main(void) {
	puts("!!!Hello Server!!!");

	Server servidor;
	start(&servidor, 8888);
	fd_set master;
	fd_set readfds;

	FD_ZERO(&readfds);

	FD_ZERO(&master);
	FD_SET(servidor.server_socket, &master);

	puts("Waiting for connections ...");
	int activity;

	while (TRUE) {

		readfds = master;
		printf("\n monitoreando... \n");
		activity = select(servidor.higherSocketDesc + 1, &readfds, NULL, NULL,
		NULL);

		if (activity == -1) {
			perror("select");
			exit(1);
		}

		explorarConexiones(&servidor, &readfds, &master);

	}

	return EXIT_SUCCESS;
}
