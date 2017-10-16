/*
 * fs_core.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef FS_CORE_H_
#define FS_CORE_H_

#include "fs.h"
#include "utils.h"

#include <dirent.h>
#include <commons/collections/list.h>


int obtenerUltimoIndiceDirValido();
int obtenerIndiceDirPadre(char* path);
int obtenerIndiceDir(char* path);
int crearDirectorio(char* nombreDir);

void formatear();

void crearBitMapBloquesNodo(t_nodo* nodo);
t_bitarray* obtenerBitMapBloquesNodo(t_nodo* nodo);
t_list* obtenerBloquesLibres(t_nodo* nodo, int cantBloques);
char* obtenerIdNodo(char* cadena);
int obtenerIdBloque(char* cadena);

t_archivoInfo* obtenerArchivoInfo(char* path);
char* obtenerDirArchivo(char* path);
char* obtenerNombreArchivo(char* path);
int existeArchivo(char* path);
int escribirArchivo(char* path, char* contenido, int tipo);

#endif /* FS_CORE_H_ */
