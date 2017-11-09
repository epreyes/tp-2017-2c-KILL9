/*
 * utils.h
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_UTILS_H_
#define HEADERS_UTILS_H_

#include "worker.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>

void generateTempsFolder();																//genera directorio de temporales
char* regenerateScript(char* fileContent, FILE* script, char operation, int socket); 	//recibo el string y genero el archivo
void loadConfigs();
void createLoggers();
char* generateScriptName(char operation, int master);
void readBuffer(int socket,int size,void* destiny);
char* serializeFile(char* fileName);
void map_data_node();
char* getBlockData(int blockNumber,int size);

#endif /* HEADERS_UTILS_H_ */
