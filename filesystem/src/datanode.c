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

	int bytesEnviados=send_all(socketNodo, contenido, finBytes);

	if (bytesEnviados < 0) {
		log_error(logger, "Error en el envio a cliente");
		return -1;
	}

	log_info(logger,"Se enviaron %d bytes al nodo", bytesEnviados);

}

// Dado un id de bloque y nodo, obtiene el contenido del mismo
// Este pedido debe invocarse de forma paralela a los diferentes nodos en cuestion (con hilos en el momento de la llamada)
int leerDeDataNode(int idBloque, int socketNodo, long finBytes, int nroBloque, t_log* logger) {

	int idNodo=buscarNodoPorSocket(socketNodo);

	log_info(logger,
			"Enviando peticion a datanode id %d - bloque: %d - finBytes: %d...",
			idNodo, idBloque, finBytes);

	t_leerBloque peticion;
	peticion.idOperacion = GET_BLOQUE;
	peticion.idBloque = idBloque;
	peticion.finByte = finBytes;

	// Envio header

	if (send(socketNodo, &peticion, sizeof(int) * 3, 0) < 0) {
		log_error(logger, "Error en el envio de lectura (header) a cliente");
		return -1;
	}


	// Envio el nro de bloque para identificar a la vuelta
	send(socketNodo, &nroBloque, sizeof(int), 0);

	return 0;

	// Espero respuesta
	/*int resp = 0;

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
		return NULL;*/

}



int send_all(int s, void *buf, int len) {
	char *ptr = (char*) buf;
	int total = 0;        // how many bytes we've sent
	int bytesleft = len; // how many we have left to send
	int n;

	while (total < len) {
		n = send(s, ptr + total, bytesleft, 0);

		if (n == -1) {
			if (errno == EINTR) //continuo por interrupciones
			{
				continue;
			} else {
				break;
			}
		} else {
			total += n;
			bytesleft -= n;
		}
	}

	return n == -1 ? -1 : total; // return -1 on failure, total bytes on success
}


int recv_all(int sockfd, void *buf, size_t len, int flags) {
	size_t toread = len;
	char *bufptr = (char*) buf;

	while (toread > 0) {
		ssize_t rsz = recv(sockfd, bufptr, toread, flags);
		if (rsz <= 0)
			return rsz; /* Error or other end closed cnnection */

		toread -= rsz; /* Read less next time */
		bufptr += rsz; /* Next buffer position to read into */
	}

	return len;
}
