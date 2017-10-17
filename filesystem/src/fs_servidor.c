/*
 * fs_servidor.c

 *
 *  Created on: 15/10/2017
 *      Author: utnso
 */

#include "fs_servidor.h"

void lanzarHiloServidor() {

	int sockfd, new_fd; // Escuchar sobre sock_fd, nuevas conexiones sobre new_fd
	struct sockaddr_in my_addr;    // información sobre mi dirección
	struct sockaddr_in their_addr; // información sobre la dirección del cliente
	int sin_size;
	int yes = 1;

	uint16_t codigoHandshake;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	my_addr.sin_family = AF_INET;         // Ordenación de bytes de la máquina
	my_addr.sin_port = htons(5002); // short, Ordenación de bytes de la red
	my_addr.sin_addr.s_addr = INADDR_ANY; // Rellenar con mi dirección IP
	memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))
			== -1) {
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	log_info(logger, "YamaFS inicializado y esperando conexiones");
	while (1) {  // main accept() loop
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size))
				== -1) {
			perror("accept");
			continue;
		}
		log_info(logger, "server: nuevo cliente conectado desde %s\n",
				inet_ntoa(their_addr.sin_addr));

		t_header d;

		if (recv(new_fd, &d, sizeof(t_header), 0) < 0) {
			log_error(logger, "Error en la recepcion del handshake");
			exit(1);
		}

		log_debug(logger, "Se recibio handshake %d", d.idMensaje);

		if (codigoHandshake != YAMA_HSK || codigoHandshake != NODO_HSK) {
			log_error(logger, "Codigo incorrecto de Handshake.");
			exit(1);
		}

		// Creo thread cliente

		pthread_t hilo_cliente;


		if (codigoHandshake == NODO_HSK) {

			log_info(logger, "Nuevo nodo conectado");

			if (pthread_create(&hilo_cliente, NULL, connection_handler_nodo,
					(void*) new_fd) < 0) {
				perror("Error creando thread cliente nodo");
				exit(1);
			}
		}


		if (codigoHandshake == YAMA_HSK) {

			log_info(logger, "Yama conectado");

			if (pthread_create(&hilo_cliente, NULL, connection_handler_yama,
					(void*) new_fd) < 0) {
				perror("Error creando thread cliente yama");
				exit(1);
			}
		}

	}

}

// Handler de conexiones de clientes

void *connection_handler_nodo(void *socket_desc) {
	int socketCliente = (int) socket_desc;
	int32_t bytes = 0;
	t_header header;

	while (1) {

		log_info(logger, "Esperando mensaje del cliente %d...", socketCliente);
		bytes = recv(socketCliente, &header, sizeof(t_header), 0);

		if (bytes <= 0) {
			log_info(logger, "Error: el cliente %d cerro la conexion",
					socketCliente);
			return 0;
		}

		procesarPedidoNodo(header, socketCliente);
	}

	return 0;
}

void *connection_handler_yama(void *socket_desc) {
	int socketCliente = (int) socket_desc;
	int32_t bytes = 0;
	t_header header;

	while (1) {

		log_info(logger, "Esperando mensaje del cliente %d...", socketCliente);
		bytes = recv(socketCliente, &header, sizeof(t_header), 0);

		if (bytes <= 0) {
			log_info(logger, "Error: el cliente %d cerro la conexion",
					socketCliente);
			return 0;
		}

		procesarPedidoYama(header, socketCliente);
	}

	return 0;
}

void procesarPedidoYama(t_header pedido, int socket) {

	void* respuesta;
	char* buffer;
	buffer = malloc(pedido.size);

	t_archivoInfo* resultado;

	switch (pedido.idMensaje) {

	case YAMA_ARCHIVO_INFO:

		resultado = obtenerArchivoInfo(buffer);

		if (resultado == ERROR1) {

			memcpy(respuesta,"",0);

			if (send(socket, &respuesta, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de archivo info a YAMA");
				exit(1);
			}

		} else {

			memcpy(respuesta,"",0);

			if (send(socket, &respuesta, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de RESPUESTA_OK a cliente");
				exit(1);
			}
		}

		break;

	default:
		// Si es invalido->no hago nada y cierro el socket
		log_error("Pedido %d invalido", pedido.idMensaje);
		close(socket);
		break;
	}

}

void procesarPedidoNodo(t_header pedido, int socket) {

	t_header respuesta;
	char* buffer;
	buffer = malloc(pedido.size);

	int resultado;

	switch (pedido.idMensaje) {

	case YAMA_ARCHIVO_INFO:

		// TODO: debe hacerse un recvall
		if (recv(socket, buffer, pedido.size, 0) < 0) {
			log_error(logger, "Error en la recepcion del buffer");
			exit(1);
		}

		resultado = 0;

		if (resultado != 0) {

			respuesta.idMensaje = -1;

			if (send(socket, &respuesta, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de error de escritura a cliente");
				exit(1);
			}

		} else {
			respuesta.idMensaje = RESPUESTA_OK; // Ok

			if (send(socket, &respuesta, sizeof(t_header), 0) < 0) {
				log_error(logger,
						"Error en el envio de respuesta de RESPUESTA_OK a cliente");
				exit(1);
			}
		}

		break;

	default:
		// Si es invalido->no hago nada y cierro el socket
		log_error("Pedido %d invalido", pedido.idMensaje);
		//close(socket);
		break;
	}

}