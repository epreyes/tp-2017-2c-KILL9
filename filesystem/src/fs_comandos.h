/*
 * fs_comandos.h
 *
 *  Created on: 20/10/2017
 *      Author: proyectomacro
 */

#ifndef FS_COMANDOS_H_
#define FS_COMANDOS_H_

#include "fs_core.h"
#include "datanode.h"
#include "estructuras.h"
#include "nodoUtils.h"

int escribirArchivo(char* path, char* contenido, int tipo, int tamanio);
char* leerArchivo(char* path);
int copiarDesdeYamaALocal(char* origen, char* destino);

#endif /* FS_COMANDOS_H_ */
