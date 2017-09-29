/*
 * yamaProcess.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#include "yamaProcess.h"

void* processOperation(Yama* yama, int master, t_header head, void* payload) {

	//me traigo la informacion del archivo.
	t_fileInfo* fsInfo = getFileInfo(yama, head, payload, master);
	free(payload);
	//proceso la informacion
	void* masterRS = processFileInfo(yama, head.op, fsInfo, master);

	//me fijo el tamaño del paquete a enviar
	t_header* fsInfoHeader = malloc(sizeof(t_header));
	memcpy(fsInfoHeader, masterRS, sizeof(t_header));
	int sizepack = sizeof(t_header) + fsInfoHeader->msg_size;

	//retorno el paquete
	return masterRS;
}

void* processFileInfo(Yama* yama, int op, t_fileInfo* fsInfo, int master){
	void* response = NULL;

	switch( op ){
	case 'T':
		response = processTransformation(yama, fsInfo, master);
		break;
	case 'L':
		//response = processLocalReduction(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
			break;
	case 'G':
		//response = processGlobalReduction(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
			break;
	case 'S':
		//response = processFinalStore(yama, &fsInfoHeader,(fsInfo + sizeof(t_header)), master);
			break;
	}
	return response;
}
