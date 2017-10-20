/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"


int transformBlock(int position, int size, char temporal[28] ){
	log_info(logger,"Bloque %d: Transformando", position);
	int result;
	//void* param = getBlockInfo(position, size);

	system("chmod 555 script.sh");					//pasar nombre de archivo a dinámico
	char command[45];
	strcpy(command,"./script.sh > ");				//pasar nombre de archivo a dinámico
	strcat(command, temporal+1);
	printf("\n\ncommand%s\n\n", command);
	result=system(command);
	if (result!=0){
		log_error(logger,"Bloque %d: Falló la transformación", position);
		return 1;
	}else{
		log_info(logger,"Bloque %d: Transformación finalizada", position);
		return 0;		//1:error al procesar el bloque
	}

}
