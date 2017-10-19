/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"


int transformBlock(int position, int size, char temporal[28] ){
	int result = 0;
	puts("estoy transformando");
	//void* param = getBlockInfo(position, size);
	char command[45];
	strcpy(command,"./transform.sh > ");
	strcat(command, temporal);
	printf("%s\n",command);
	result=system(command);
	printf("%d\n",result);
	return (result==0)?0:1;		//1:error al procesar el bloque
}
