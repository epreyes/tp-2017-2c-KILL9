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
bool existeDirectorio(char* path);

void formatear();

void crearBitMapBloquesNodo(t_nodo* nodo);
t_bitarray* obtenerBitMapBloquesNodo(t_nodo* nodo);
t_list* obtenerBloquesLibres(int cantBloques);
char* obtenerIdNodo(char* cadena);
int obtenerIdBloque(char* cadena);
int obtenerBloquesNecesarios(char* contenido, int tipo);

t_archivoInfo* obtenerArchivoInfo(char* path);
char* obtenerDirArchivo(char* path);
char* obtenerNombreArchivo(char* path);
int existeArchivo(char* path);
int crearArchivoMetadata(t_list* bloquesInfo, char* path, int tipo, int tamanio);
int leerArchivo(char* path);
void actualizarBitMapBloques(t_list* bloquesInfo);

bool ordenarPorCarga(t_nodo *nodoConMasCarga, t_nodo *nodoConMenosCarga);
t_list* decidirNodo(t_list* nodos);

bool estaFormateado();

#endif /* FS_CORE_H_ */
