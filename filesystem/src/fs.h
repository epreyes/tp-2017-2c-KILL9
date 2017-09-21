/*
 * fs.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef FS_H_
#define FS_H_

#include <arpa/inet.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>


// Estructuras administrativas del FS

#define TAMANIO BLOQUE 1 // 1 MB
#define MAX_DIR_FS 100

typedef struct {
	int tamanio;
	int tipo;
	int dirPadre;
	short int estado;
	t_list* bloques;
} t_archivo;

typedef struct {
	;
} t_bloque;

typedef struct {
	int indice;
	char nombre[255];
	int padre;
} t_directorio;


#include "utils.h"
#include "fs_core.h"

void iniciarFS();

#endif /* FS_H_ */
