/*
 * utils.c
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#include "fs.h"
#include "utils.h"

void listarDirectorios(t_directorio* inicioTablaDirectorios) {

	t_directorio* dir = inicioTablaDirectorios;
	int i=0;

	for (i=0;i<MAX_DIR_FS;i++) {


		if (dir->indice==0 && dir->padre==-1)
		printf("%d Directorio raiz - Dir padre: %d\n", dir->indice,
				dir->padre);

		if (dir->indice!=0 && dir->padre>=0)
				printf("%d Directorio: %s - Dir padre: %d\n", dir->indice, dir->nombre,
						dir->padre);


		dir++;
	}

}
