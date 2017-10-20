/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"

char transformBlock(int position, int size, char temporal[28] ){
	log_info(logger,"Bloque %d: Transformando", position);

	//void* param = getBlockInfo(position, size);

	system("chmod 555 script.sh");					//pasar nombre de archivo a dinámico
	char command[45];
	strcpy(command,"./script.sh > ");				//pasar nombre de archivo a dinámico
	strcat(command, temporal+1);
	if (system(command)!=0){
		log_error(logger,"Bloque %d: Falló la transformación", position);
		return 'E';
	}else{
		log_info(logger,"Bloque %d: Transformación finalizada", position);
		return 'O';
	}
}
