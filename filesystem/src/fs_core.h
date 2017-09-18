/*
 * fs_core.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef FS_CORE_H_
#define FS_CORE_H_

#include <dirent.h>
#include "fs.h"
#include <commons/collections/list.h>

int obtenerUltimoIndiceDirValido(t_directorio* inicioTablaDirectorios);
int obtenerIndiceDirPadre(char* path, t_directorio* inicioTablaDirectorios);
int obtenerIndiceDir(char* path, t_directorio* inicioTablaDirectorios);

int crearDirectorio(t_directorio* inicioTablaDirectorios, char* nombreDir);
void formatear(t_directorio* inicioTablaDirectorios);


t_list* listarArchivos(char* path, t_directorio* inicioTablaDirectorios);

#endif /* FS_CORE_H_ */
