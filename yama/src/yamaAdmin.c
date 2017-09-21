/*
 ============================================================================
 Name        : yamaAdmin.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "yamaAdmin.h"
#include "yamaProcess.h"

/*---------------------- Private ---------------------------------*/
t_config* getConfig(Yama* yama) {
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

void setProperties(Yama* yama, t_config* config) {

	yama->port = config_get_int_value(config, "YAMA_PUERTO");
	yama->fs_port = config_get_int_value(config, "FS_PUERTO");
	yama->planning_delay = config_get_int_value(config,
			"RETARDO_PLANIFICACION");

	strcpy(yama->fs_ip, config_get_string_value(config, "FS_IP"));
	strcpy(yama->balancign_algoritm,
			config_get_string_value(config, "ALGORITMO_BALANCEO"));

	yama->log = configLog();
	yama->stateTable = list_create();
	log_trace(yama->log, "YAMA was succesfully configured.");
}

/*---------------------- Public ----------------------------------*/
Yama configYama() {
	Yama yama;
	t_config* config = getConfig(&yama);

	setProperties(&yama, config);

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
		processRequest(yama, request, socket, nbytes);
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
