/*
 * connectionManager.c
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#include "headers/connectionManager.h"
#include "headers/yama.h"

Client acceptMasterConnection(Server* server, fd_set* masterList, int hightSd) {
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);
	Client theClient;

	int client = accept(server->server_socket,
			(struct sockaddr *) &client_address, &addrlen);

	if (client > -1) {
		FD_SET(client, &(*masterList));
		if (client > hightSd) {
			server->higherSocketDesc = client;
		}
		theClient.socket_id = client;
		theClient.address = client_address;
		yama->jobs++;
	} else {
		log_error(yama->log, "Error aceptando conexion.");
	}

	return theClient;
}

int getSizeToSend(void* masterRS) {
	char op;
	memcpy(&op, masterRS, sizeof(char));

	int blocks;
	memcpy(&blocks, masterRS + sizeof(char), sizeof(int));

	int size = 0;
	int grSize = sizeof(char) * 69 + sizeof(int) * 2;
	int fsSize = sizeof(char) * 40 + sizeof(int) * 2;

	switch (op) {
	case 'T':
		size = (blocks * sizeof(tr_datos)) + sizeof(char) + sizeof(int);
		break;
	case 'L':
		size = (blocks * sizeof(rl_datos)) + sizeof(char) + sizeof(int);
		break;
	case 'G':
		size = (blocks * grSize) + sizeof(char) + sizeof(int);
		break;
	case 'S':
		size = fsSize + sizeof(char);
		break;
	case 'A':
		size = sizeof(error_rq);
		break;
	case 'E':
		size = sizeof(char);
		break;
	case 'R': {
		char opCode;
		memcpy(&opCode, masterRS + sizeof(char), sizeof(char));

		int blocks;
		memcpy(&blocks, masterRS + sizeof(char) + sizeof(char), sizeof(int));
		size = (2 * sizeof(char)) + sizeof(int) + (sizeof(tr_datos) * blocks);
	}
		break;
	}

	return size;
}

int sendResponse(int master, void* masterRS, t_job* job) {

	int bytesSent = send(master, masterRS, getSizeToSend(masterRS), 0);

	if (bytesSent > 0) {
		sendResponseMsg(master, bytesSent, masterRS, job);
		free(masterRS);
	} else {
		log_error(yama->log, "Error al enviar la respuesta al Master (%d).",
				master);
	}
	return bytesSent;
}

int getMasterMessage(int socket, fd_set* mastersList) {
	void* request = malloc(sizeof(char));
	int nbytes = recv(socket, request, sizeof(char), MSG_WAITALL);
	/* Si recibo -1 o 0, el cliente se desconecto o hubo un error */
	if (nbytes <= 0) {
		if (nbytes == 0) {
			log_trace(yama->log, "Master %d desconectado!", socket);
		} else {
			log_error(yama->log, "Error al recibir mensajes de master.");
		}
		/* si hubo error, desconecto el socket y lo saco de la lista de monitoreo */
		close(socket);
		FD_CLR(socket, &(*mastersList));
	}
	/* si recibi bytes, proceso el request */
	else {
		//proceso el request y obtengo la respuesta
		void* response = getResponse(socket, *(char*) request);
		char opRq = *(char*) request;

		char responseCode;
		memcpy(&responseCode, response, sizeof(char));

		t_job* job = list_get(yama->tabla_jobs, getJobIndex(socket, opRq));
		if (responseCode == 'R') {
			job = list_get(yama->tabla_jobs, getJobIndex(socket, responseCode));
		}

		if ((responseCode == 'T') || (responseCode == 'L')
				|| (responseCode == 'G') || (responseCode == 'S')
				|| ((responseCode == 'E') && (opRq == 'T'))
				|| ((opRq == 'E') && (responseCode = 'A'))
				|| (responseCode == 'R')) {
			sendResponse(socket, response, job);
		} else {
			if (responseCode != 'O') {
				showErrorMessage(response, job);
			}
		}

	}
	free(request);
	return nbytes;
}

void exploreActivity(fd_set* mastersListTemp, fd_set* mastersList) {
	/* creo un indice para recorrer la lista de sockets */
	int index = 0;
	Client client;

	int hightSd = yama->yama_server.higherSocketDesc;

	for (index = 0; index <= hightSd; index++) {
		if (FD_ISSET(index, &(*mastersListTemp))) {
			/* si hubo actividad en el socket servidor, alguien se conecto. */
			if (index == yama->yama_server.server_socket) {
				client = acceptMasterConnection(&(yama->yama_server),
						&(*mastersList), hightSd);

				log_trace(yama->log, "Master %d conectado!", client.socket_id);
			}
			/* Si hubo actividad en otro socket, recibo el mensage */
			else {
				getMasterMessage(index, &(*mastersList));
			}
		}
	}
}

void waitMastersConnections() {

	if (yama->yama_server.status > -1) {
		log_info(yama->log, "YAMA listo para recibir solicitudes de Masters.",
				getpid());

		/* creo las listas de sockets entrantes que seran monitoreadas */
		fd_set mastersList;
		fd_set mastersListTemp;

		/* limpio las listas y seteo el socket server */FD_ZERO(&mastersList);
		FD_ZERO(&mastersListTemp);
		FD_SET(yama->yama_server.server_socket, &mastersList);

		int activity;

		/* bucle para monitorear conexiones */
		while (TRUE) {
			mastersListTemp = mastersList;
			activity = select(yama->yama_server.higherSocketDesc + 1,
					&mastersListTemp, NULL, NULL, NULL);
			if ( errno == EINTR) {
				continue;
			}
			if (activity == -1) {
				log_error(yama->log, "Error monitoreando conexiones.");
			} else {
				/* exploro la actividad reciente */
				exploreActivity(&mastersListTemp, &mastersList);
			}
		}
	} else {
		log_info(yama->log, "Error: El servidor de yama no esta listo.");
	}

}
