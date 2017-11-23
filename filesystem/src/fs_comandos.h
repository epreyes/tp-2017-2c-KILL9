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
#include <commons/collections/dictionary.h>

int escribirArchivo(char* path, char* contenido, int tipo, int tamanio);
char* leerArchivo(char* path, int* codigoError);
int copiarDesdeYamaALocal(char* origen, char* destino);
t_list* obtenerNodosALeer(t_list* bloques, t_list* nodosExcluir);

void* eliminarItemLectura(t_lectura* lect);

int eliminarBloque(char* pathArchivo, int nroBloque, int nroCopia);
void eliminarBloqueArchivoMd(char* dirMetadata, int nroBloque, int nroCopia);
void config_remove_key(t_config * config, char *key);
char* obtenerMd5(char* pathArchivo);

#endif /* FS_COMANDOS_H_ */
