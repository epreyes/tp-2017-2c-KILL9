/*
 * filesystem_datanode.c
 *
 *  Created on: 19/10/2017
 *      Author: utnso
 */

#include "filesystem_datanode.h"
#include <stdlib.h>
#include <sys/socket.h>



t_leerBloque* recibirPaquete(int fd) {

	t_leerBloque* paqueteRecivido = malloc(sizeof(t_leerBloque));
	int ret = 0;
	ret = recv(fd, &paqueteRecivido->idOperacion, sizeof(int), MSG_WAITALL);

	if (ret == 0) {
		return NULL;
	}
	ret =recv(fd, &paqueteRecivido->idBloque, sizeof(int), MSG_WAITALL);

	if (ret == 0) {
		return NULL;
	}
	 ret = recv(fd, &paqueteRecivido->finByte, sizeof(int), MSG_WAITALL);

	if (ret == 0) {
		return NULL;
	}
	return paqueteRecivido;
}
