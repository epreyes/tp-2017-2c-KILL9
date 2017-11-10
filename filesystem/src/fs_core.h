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

int formatear();

void crearBitMapBloquesNodo(t_nodo* nodo);
t_bitarray* obtenerBitMapBloquesNodo(t_nodo* nodo); // Debe deprecarse

t_bitarray* obtenerBitMapBloques(int idNodo);

t_list* obtenerBloquesLibres(int cantBloques);
char* obtenerIdNodo(char* cadena);
int obtenerIdBloque(char* cadena);
int obtenerBloquesNecesarios(char* contenido, int tipo, int tamanio);

t_archivoInfo* obtenerArchivoInfo(char* path);
t_archivoInfo* obtenerArchivoInfoPorMetadata(char* dirMetadata);
char* obtenerDirArchivo(char* path);
char* obtenerNombreArchivo(char* path);
int existeArchivo(char* path);
int crearArchivoMetadata(t_list* bloquesInfo, char* path, int tipo, int tamanio);
void actualizarBitMapBloques(t_list* bloquesInfo);

bool ordenarPorCarga(t_nodo *nodoConMasCarga, t_nodo *nodoConMenosCarga);
t_list* decidirNodo(t_list* nodos);

bool estaFormateado();
int obtenerEstadoArchivo(char* path);

#endif /* FS_CORE_H_ */
