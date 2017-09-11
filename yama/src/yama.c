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

/*---------------------- Private ---------------------------------*/
t_config* getConfig() {
	t_config* config = config_create(CONFIG_PATH);
	return config;
}

t_log* configLog() {
	return log_create(LOG_PATH, "YAMA", 1, LOG_LEVEL_TRACE);
}

void setProperties(Yama* yama, t_condig* config) {
	yama->port = config_get_int_value(config, "YAMA_PUERTO");
	yama->fs_port = config_get_int_value(config, "FS_PUERTO");
	yama->planning_delay = config_get_int_value(config, "RETARDO_PLANIFICACION");

	strcpy(yama->fs_ip, config_get_string_value(config, "FS_IP"));
	strcpy(yama->balancign_algoritm, config_get_string_value(config, "ALGORITMO_BALANCEO"));

	yama->log = configLog();
}

/*---------------------- Public ----------------------------------*/
Yama configYama() {
	Yama yama;
	t_config* config = getConfig();

	setProperties(&yama, config);

	return yama;
}

Client acceptMasterConnection(Yama* yama, Server* server, fd_set* masterList, int hightSd) {
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);
	int client = accept(server->server_socket,
			(struct sockaddr *) &client_address, &addrlen);
	if (client == -1) {
		perror("accept");
	} else {
		log_trace(yama->log, "New Master connection");
		FD_SET(client, &(*masterList));
		if (client > hightSd) {
			server->higherSocketDesc = client;
		}
		printf("selectserver: new connection from %s on socket %d\n",
				inet_ntoa(client_address.sin_addr), client);
	}
	Client theClient;
	theClient.socket_id = client;
	theClient.address = client_address;

	return theClient;
}

int processRequest(Yama* yama, void* buff, int socket, int nbytes) {
	log_trace(yama->log, "Process Master Request");
	return 1;
}

int getMasterMessage(Yama* yama, int socket, int* nbytes, fd_set* mastersList) {
	void* buff = malloc(sizeof(header));
	*nbytes = recv(socket, buff, sizeof(header), 0);
	if (*nbytes <= 0) {
		if (*nbytes == 0) {
			log_trace(yama->log, "Master disconnected");
			printf("se desconecto el socket %d \n", socket);
		} else {
			perror("recv");
		}
		close(socket);
		FD_CLR(socket, &(*mastersList));
	}

	else {
		log_trace(yama->log, "Getting Master message...");
		printf("\nRecibi %d bytes\n", *nbytes);
		processRequest(yama, buff, socket, *nbytes);
	}
	return *nbytes;
}

void exploreMastersConnections(Yama* yama, fd_set* mastersListTemp,
		fd_set* mastersList) {
	int i = 0;
	Client client;
	int nbytes = 0;
	int hightSd = yama->yama_server.higherSocketDesc;

	for (i = 0; i <= hightSd; i++) {
		if (FD_ISSET(i, &(*mastersListTemp))) {
			log_trace(yama->log, "Connection incomming");
			if (i == yama->yama_server.server_socket) {
				client = acceptMasterConnection(yama, &(yama->yama_server),
						&(*mastersList), hightSd);
				log_trace(yama->log, "Master process connected!");
				printf("\nse me conecto el socket %d\n", client.socket_id);
			} else {
				printf("\n\nRecibi %d bytes de client %d!\n\n",
						getMasterMessage(yama, i, &nbytes, &(*mastersList)), i);
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
			perror("select");
			exit(1);
		}

		exploreMastersConnections(yama, &mastersListTemp, &mastersList);
	}
}
