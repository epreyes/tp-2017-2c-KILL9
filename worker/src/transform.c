/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"

char transformBlock(int position, int size, char temporal[28], char* scriptName){
	log_info(logger,"Bloque %d: Transformando", position);

	char* blockContent;
	//blockContent = getBlockInfo(position, size);

	char command[59];
	strcpy(command, "./");
	strcat(command, scriptName);
	strcat(command, " > ");
	strcat(command, temporal+1);

	if (system(command)!=0){
		log_error(logger,"Bloque %d: Falló la transformación", position);
		return 'E';
	}else{
		log_info(logger,"Bloque %d: Transformación finalizada", position);
		return 'O';
	}
}
