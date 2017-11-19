/*
 * connectionsManager.c
 *
 *  Created on: 16/10/2017
 *      Author: utnso
 */

#include "headers/connectionsManager.h"

void loadServer(void) {

	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = INADDR_ANY;
	workerAddr.sin_port = htons(config_get_int_value(config, "WORKER_PUERTO"));
	int workerSocket = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(workerSocket, SOL_SOCKET, SO_REUSEADDR, &activado,
			sizeof(activado));

	if (bind(workerSocket, (void*) &workerAddr, sizeof(workerAddr)) != 0) {
		log_error(logger, "No se pudo realizar el bind en puerto %d",
				config_get_int_value(config, "WORKER_PUERTO"));
		exit(1);
	}
	log_trace(logger, "Worker iniciado en puerto %d",
			config_get_int_value(config, "WORKER_PUERTO"));
	listen(workerSocket, 100);

	struct sockaddr_in clientAddr;
	unsigned int len;
	while (1) {
		//acepto conexion de algun cliente
		log_trace(logger, "Esperando conexiones...");
		int clientSocket = accept(workerSocket, (void*) &clientAddr, &len);

		//si hubo conexion
		if (clientSocket != -1) {
			log_trace(logger, "Cliente %d: Conectado", clientSocket);
			socket_worker = workerSocket;

			//forkeo para atender solicitud
			int pid = fork();
			if (pid < 0) {
				perror("ERROR on fork");
				exit(1);
			}
			//si estoy en el proceso hijo, atiendo solicitud
			if (pid == 0) {
				socket_master = clientSocket;
				while (readClientBuffer() > 0);
				exit(1);
			}
		} else {
			log_error(logger, "Error al establecer conexión con Cliente");
			exit(1);
		}

	} /* end of while */

}

//=======================FILESYSTEM=============================================

void openFileSystemConnection(void) {
	struct sockaddr_in fileSystemAddr;
	char* fileSystem_ip = config_get_string_value(config, "IP_FILESYSTEM");
	int fileSystem_port = config_get_int_value(config, "PUERTO_FILESYSTEM");
	fileSystemAddr.sin_family = AF_INET;
	fileSystemAddr.sin_addr.s_addr = inet_addr(fileSystem_ip);
	fileSystemAddr.sin_port = htons(fileSystem_port);

	socket_filesystem = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(socket_filesystem, (void*) &fileSystemAddr, sizeof(fileSystemAddr)) != 0) {
		log_warning(logger, "No se pudo conectar con Filesystem (%s:%d)", fileSystem_ip, fileSystem_port);
		exit(1);
	};
	log_info(logger, "Conexión con Filesystem establecida (%s:%d)", fileSystem_ip, fileSystem_port);
}

//=======================NODE=============================================
int openNodeConnection(int node, char* ip, int port) {

	struct sockaddr_in workerAddr;
	workerAddr.sin_family = AF_INET;
	workerAddr.sin_addr.s_addr = inet_addr(ip);
	workerAddr.sin_port = htons(port);

	socket_nodes[node] = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(socket_nodes[node], (void*) &workerAddr, sizeof(workerAddr))
			!= 0) {
		log_error(logger, "No se puede conectar con el nodo:%d (%s:%d)", node,
				ip, port);
		return 1;
	} else {
		log_info(logger,
				"Conexión con nodo %d establecida (puerto:%d-socket:%d)", node,
				port, socket_nodes[node]);
		return 0;
	}
}

//==========================================================================

int readClientBuffer() {
	char operation;
	if (readBuffer(socket_master, sizeof(char), &operation) == 0) {
		printf("SOLICITUD:%c",operation);
		switch (operation) {
		case 'T':
			log_trace(logger, "Master %d: Solicitud de transformación recibida",
					socket_master);
			transformation();
			return 1;
			break;
		case 'L':
			log_info(logger, "Master %d: Solicitud de Reducción Local recibida",
					socket_master);
			localReduction();
			return 1;
			break;
		case 'G':
			log_info(logger,
					"Master %d: Solicitud de Reducción Global recibida",
					socket_master);
			globalReduction();
			return 1;
			break;
		case 'S':
			log_info(logger,
					"Master %d: Solicitud de Almacenado Final recibida",
					socket_master);
			finalStorage();
			return 1;
			break;
		case 'R':
			log_trace(logger,
					"Worker %d: Solicitud de archivo temporal recibida",
					socket_worker);
			sendNodeFile(socket_worker);
			return 1;
			break;
		default:
			log_warning(logger, "No existe la operación Solicitada");
			return -1;
			break;
		}
	}
	else{
		return -1;
	}
}
