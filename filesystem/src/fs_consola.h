/*
 * fs_consola.h
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#ifndef FS_CONSOLA_H_
#define FS_CONSOLA_H_

#define ESCRIBIR_ARCHIVO_LOCAL_YAMA "cpfrom"
#define LISTAR_ARCHIVOS "ls"
#define AYUDA "ayuda"
#define CREAR_DIRECTORIO "mkdir"
#define FORMATEAR "formatear"
#define INFOARCHIVO "infoarchivo"
#define ESTADONODOS "estadoNodos"
#define LEERARCHIVO "cat"
#define COPIARDEYAMAALOCAL "cpto"
#define ELIMINAR "rm"
#define MD5SUM "md5sum"
#define RENOMBRAR "rename"

#include "fs_comandos.h"

void ejecutarConsola();

#endif /* FS_CONSOLA_H_ */

