/*
 * utils.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#include "headers/utils.h"

void generateTempsFolder(){
	system("mkdir -p tmp");
};

void regenerateScript(char* fileContent, FILE* script, char* scriptName){
	script = fopen("transform2.sh","w"); //ver si es necesario que le pase el nombre por el .sh en el socket
	fprintf(script,fileContent);
};
