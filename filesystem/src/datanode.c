/*
 * datanode.c
 *
 * Pedidos al datanode
 *
 *  Created on: 19/10/2017
 *      Author: utnso
 */

#include "datanode.h"

// Pedidos al datanode

// Dado un id de bloque y un contenido, escribe el mismo en el datanode
int escribirEnDataNode(int idBloque, char* contenido, int socketNodo,
		long finBytes, t_log* logger) {

	if (DATANODE_DUMMY == 1) {
		log_info(logger,
				"Enviando peticion a datanode %d - bloque: %d - finBytes: %d...",
				socketNodo, idBloque, finBytes);

		log_info(logger, "Peticion a datanode ok...");

		return 0;
	}

	log_info(logger,
			"Enviando peticion a datanode %d - bloque: %d - finBytes: %d...",
			socketNodo, idBloque, finBytes);

	t_escribirBloque peticion;
	peticion.idOperacion = SET_BLOQUE;
	peticion.idBloque = idBloque;
	peticion.finByte = finBytes;

	// Envio header

	if (send(socketNodo, &peticion, sizeof(int) * 3, 0) < 0) {
		log_error(logger, "Error en el envio de escritura (header) a cliente");
		return -1;
	}

	// TODO: hacer sendall
	if (send(socketNodo, contenido, finBytes, 0) < 0) {
		log_error(logger, "Error en el envio a cliente");
		return -1;
	}

}

// Dado un id de bloque y nodo, obtiene el contenido del mismo
// Este pedido debe invocarse de forma paralela a los diferentes nodos en cuestion (con hilos en el momento de la llamada)
char* leerDeDataNode(int idBloque, int socketNodo, long finBytes, t_log* logger) {
	log_info(logger,
			"Enviando peticion a datanode %d - bloque: %d - finBytes: %d...",
			socketNodo, idBloque, finBytes);

	t_leerBloque peticion;
	peticion.idOperacion = GET_BLOQUE;
	peticion.idBloque = idBloque;
	peticion.finByte = finBytes;

	char* cont = malloc(sizeof(char) * finBytes);

	// Envio header

	if (send(socketNodo, &peticion, sizeof(t_escribirBloque), 0) < 0) {
		log_error(logger, "Error en el envio de lectura (header) a cliente");
		return NULL;
	}

	// Espero respuesta
	int resp = 0;

	if (recv(socketNodo, resp, sizeof(int), 0) < 0) {
		log_error(logger, "Error en la recepcion de info del nodo");
		return NULL;
	}

	if (resp == GET_BLOQUE_OK) {
		// TODO: hacer recvall
		if (recv(socketNodo, cont, finBytes, 0) < 0) {
			log_error(logger, "Error en la recepcion de info del nodo");
			return NULL;
		}
		log_info(logger, "Peticion a datanode ok...");
		return cont;
	} else
		return NULL;

}

