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
#include "yamaProcess.h"

/*---------------------- Private ---------------------------------*/
t_config* getConfig() {
	t_config* config;
	if (fileExist(CONFIG_PATH)) {
		config = config_create(CONFIG_PATH);
	} else {
		perror("Missing configuration file.");
		exit(1);
	}
	return config;
}

t_log* configLog() {
	return log_create(LOG_PATH, "YAMA", 1, LOG_LEVEL_TRACE);
}

void setProperties(Yama* yama) {
	yama->work_charge = 0;
	yama->log = configLog();
	yama->state_table = list_create();
	yama->node_state_table = list_create();
	yama->file_info = list_create();
	log_trace(yama->log, "YAMA was succesfully configured.");
}

/*---------------------- Public ----------------------------------*/
Yama configYama() {
	Yama yama;
	yama.config = getConfig(&yama);
	setProperties(&yama);

	return yama;
}

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList,
		int hightSd) {
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);
	Client theClient;

	int client = accept(server->server_socket,
			(struct sockaddr *) &client_address, &addrlen);

	if (client > -1) {
		log_trace(yama->log, "New Master connection");
		FD_SET(client, &(*masterList));
		if (client > hightSd) {
			server->higherSocketDesc = client;
		}
		theClient.socket_id = client;
		theClient.address = client_address;
	} else {
		log_trace(yama->log, "Error trying to accept connection");
		perror("Error trying to accept connection");
		exit(1);
	}

	return theClient;
}

t_package* getRequest(Yama* yama, void* request, int master) {
	log_trace(yama->log, "Processing Master Request...");

	t_header* head = malloc(sizeof(t_header));
	memcpy(head, request, sizeof(t_header));

	void* payload = malloc(head->msg_size);
	recv(master, payload, head->msg_size, 0);

	t_package* response = malloc(sizeof(t_package));
	response->head = *(head);
	response->payload = malloc(head->msg_size);
	memcpy(response->payload, payload, head->msg_size);

	free(head);
	free(payload);

	return response;
}

void* getResponse(Yama* yama, t_package* request, int master) {
	t_header head = request->head;
	void* payload = malloc(head.msg_size);
	memcpy(payload, request->payload, head.msg_size);
	return processOperation(yama, master, head, payload);
}

int sendResponse(Yama* yama, int master, void* masterRS) {
	//me fijo el tamaño del paquete a enviar
	t_header* fsInfoHeader = malloc(sizeof(t_header));
	memcpy(fsInfoHeader, masterRS, sizeof(t_header));
	int sizepack = sizeof(t_header) + fsInfoHeader->msg_size;

	//envio el paquete y retorno
	return sendPackage(yama, master, masterRS, sizepack);
}

int getMasterMessage(Yama* yama, int socket, fd_set* mastersList) {
	void* request = malloc(sizeof(t_header));
	int nbytes = recv(socket, request, sizeof(t_header), 0);

	if (nbytes <= 0) {
		if (nbytes == 0) {
			log_trace(yama->log, "Master disconnected.");
		} else {
			log_trace(yama->log, "Error getting data from Master");
			perror("recv");
		}
		close(socket);
		FD_CLR(socket, &(*mastersList));
	} else {
		log_trace(yama->log, "Getting Master message...");
		//recivo el mensaje
		t_package* requestRecived = getRequest(yama, request, socket);
		free(request);
		//lo proceso y obtengo la respuesta
		void* response = getResponse(yama, requestRecived, socket);

		//envio el paquete al master
		sendResponse(yama, socket, response);
	}
	return nbytes;
}

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp,
		fd_set* mastersList) {
	int i = 0;
	Client client;

	int hightSd = yama->yama_server.higherSocketDesc;

	for (i = 0; i <= hightSd; i++) {
		if (FD_ISSET(i, &(*mastersListTemp))) {
			if (i == yama->yama_server.server_socket) {
				client = acceptMasterConnection(yama, &(yama->yama_server),
						&(*mastersList), hightSd);
				log_trace(yama->log, "Master process connected!");
			} else {
				getMasterMessage(yama, i, &(*mastersList));
			}
		}

	}
}

void waitForMasters(Yama* yama) {

	if (yama->yama_server.status > -1) {
		log_trace(yama->log, "YAMA ready to accept Masters connections");
	}

	fd_set mastersList;
	fd_set mastersListTemp;

	FD_ZERO(&mastersList);
	FD_ZERO(&mastersListTemp);
	FD_SET(yama->yama_server.server_socket, &mastersList);

	puts("Waiting for Masters connections...");
	int activity;

	while (TRUE) {
		mastersListTemp = mastersList;
		activity = select(yama->yama_server.higherSocketDesc + 1,
				&mastersListTemp, NULL, NULL, NULL);

		if (activity == -1) {
			log_trace(yama->log, "Error monitoring current connections.");
			perror("select");
			exit(1);
		}

		exploreMastersConnections(yama, &mastersListTemp, &mastersList);
	}
}
