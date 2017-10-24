/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"

char transformBlock(int position, int size, char temporal[28], char* scriptName){
	log_info(logger,"Bloque %d: Transformando...", position);

	//-----Obtengo contenido del bloque-----------------------------
	char* blockContent = NULL;
	blockContent = malloc(size);
	asprintf(&blockContent, "\"Contenido del bloque %d\"",position);
	printf("\tCONTENT:%s\n", blockContent);
	//blockContent = getBlockInfo(position, size);
	//--------------------------------------------------------------
	//sizeBlock+temp[26]+command[12]+temp[28]+\0[1]
	char* command = NULL;
	command = malloc(size+26+12+28+1);
	asprintf(&command, "echo %s | %s > %s",blockContent, scriptName, temporal+1);
	printf("\tCOMMAND:%s\n", command);

	free(blockContent);

	if (system(command)!=0){
		log_error(logger,"Bloque %d: Falló la transformación", position);
		free(command);
		return 'E';
	}else{
		log_info(logger,"Bloque %d: Transformación finalizada", position);
		free(command);
		return 'O';
	}
}
