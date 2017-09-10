/*
 ============================================================================
 Name        : pruebaCliente.c
 Author      : 
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "Socket.h"

int main(void) {
	puts("!!!Hello Client!!!"); /* prints !!!Hello World!!! */

	Client client1;
	connectClient(&client1, "127.0.0.1", 8888);

	typedef struct header{
		int tipo;
		int tam;
	}header;
	//tipo 1 persona, tipo 2 empresa

	//serializacion: primeros 4 son el dni y segundos 20 son el nombre;
	typedef struct estructura{
		char nombre[20];
		int dni;
	}estructura;

	header h;
	h.tipo=1;
	h.tam = sizeof(estructura);

	estructura persona;
	persona.dni = 1237;
	strcpy(persona.nombre, "Cuchu");

	void* buffer = malloc(sizeof(h)+sizeof(persona));
	memcpy(buffer, &h, sizeof(h));
	memcpy(buffer+sizeof(h), &persona, sizeof(persona));

	send(client1.sock_server_id, buffer, sizeof(h)+sizeof(persona), 0 );
	/*---------------------------------------------------------------------------------------------------------------------------------*/

	Client client2;
		connectClient(&client2, "127.0.0.1", 8888);

		//tipo 1 persona, tipo 2 empresa

		//serializacion: primeros 4 son el dni y segundos 20 son el nombre;
		typedef struct empre{
			char nombre[20];
			int cuit;
			int tele;
		}empre;

		h.tipo=2;
		h.tam = sizeof(estructura);

		empre empresa;
		empresa.cuit = 5544;
		empresa.tele = 111222333;
		strcpy(empresa.nombre, "SRL");

		buffer = malloc(sizeof(h)+sizeof(empresa));
		memcpy(buffer, &h, sizeof(h));
		memcpy(buffer+sizeof(h), &empresa, sizeof(empresa));

		send(client2.sock_server_id, buffer, sizeof(h)+sizeof(empresa), 0 );

	return EXIT_SUCCESS;
}
