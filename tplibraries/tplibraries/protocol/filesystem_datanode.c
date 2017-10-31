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
	recv(fd, &paqueteRecivido->idBloque, sizeof(int), MSG_WAITALL);
	printf("idb: %d\n",paqueteRecivido->idBloque);
	recv(fd, &paqueteRecivido->finByte, sizeof(int), MSG_WAITALL);
	printf("finb: %d\n",paqueteRecivido->finByte);
	paqueteRecivido->contenido=malloc(sizeof(char)* paqueteRecivido->finByte);
	recv(fd, paqueteRecivido->contenido,sizeof(char)* paqueteRecivido->finByte, MSG_WAITALL);
	return paqueteRecivido;
}
