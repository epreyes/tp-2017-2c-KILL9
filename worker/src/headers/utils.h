/*
 * utils.h
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_UTILS_H_
#define HEADERS_UTILS_H_

#include "worker.h"

void generateTempsFolder();													//genera directorio de temporales
void regenerateScript(char* fileContent, FILE* script, char* scriptName); 	//recibo el string y genero el archivo
void loadConfigs();
void createLoggers();

#endif /* HEADERS_UTILS_H_ */
