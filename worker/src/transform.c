/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"


int transformBlock(int position, int size, char temporal[28] ){
	int result = 0;
	//void* param = getBlockInfo(position, size);
	char command[45];
	strcpy(command,"./transform.sh > ");
	strcat(command, temporal);
	result=system(command);
	return (result==0)?0:1;		//1:error al procesar el bloque
}
