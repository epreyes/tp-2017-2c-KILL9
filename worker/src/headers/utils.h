/*
 * utils.h
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_UTILS_H_
#define HEADERS_UTILS_H_

#include "worker.h"

void generateTempsFolder();																//genera directorio de temporales
char* regenerateScript(char* fileContent, FILE* script, char operation, int socket); 	//recibo el string y genero el archivo
void loadConfigs();
void createLoggers();
char* generateScriptName(char operation, int master);
void readSocketBuffer(int socket,int size,void* destiny);

#endif /* HEADERS_UTILS_H_ */
