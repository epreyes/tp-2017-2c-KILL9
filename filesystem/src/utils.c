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
	int i = 0;

	for (i = 0; i < MAX_DIR_FS; i++) {

		if (dir->indice == 0 && dir->padre == -1)
			printf("%d Directorio raiz - Dir padre: %d\n", dir->indice,
					dir->padre);

		if (dir->indice != 0 && dir->padre >= 0)
			printf("%d Directorio: %s - Dir padre: %d\n", dir->indice,
					dir->nombre, dir->padre);

		dir++;
	}

}

// Simula la entrada de un nodo al fs

void simularEntradaConexionNodo(t_nodos* nodos) {

	// Aca deberia modificarse nodos.bin

	nodos = malloc(sizeof(t_nodos));
	nodos->tamanio = 100;
	nodos->libre = 100;
	nodos->nodos = list_create();

	t_nodo* nodo1 = malloc(sizeof(t_nodo));
	nodo1->id = 3;
	nodo1->libre = 16;
	nodo1->total = 16;
	nodo1->direccion = "127.0.0.1:6001";
	nodo1->socketNodo = 1;

	nodos->nodos = list_create();

	list_add(nodos->nodos, nodo1);

	crearBitMapBloquesNodo(nodo1);

}

int obtenerProfDir(char* path) {

	char *p = path;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	return d;

}

char** obtenerDirectorios(char* path) {
	return string_split(path, "/");
}

void imprimirEstadoNodos() {

	int i = 0;
	for (i = 0; i < list_size(nodos->nodos); i++) {
		t_nodo* nod = list_get(nodos->nodos, i);
		t_bitarray* bm = obtenerBitMapBloques(nod->id);

		printf("nodo %d | tamanio: %d | libre: %d\n", nod->id, nod->total,
				nod->libre);

		int j = 0;
		for (j = 0; j < bm->size; j++) {
			if (bitarray_test_bit(bm, j))
				printf("1");
			else
				printf("0");
		}
		printf("\n");

	}
	printf("Espacio total: %d bytes\n", nodos->tamanio * TAMANIO_BLOQUE);
	printf("Espacio libre: %d bytes\n\n", nodos->libre * TAMANIO_BLOQUE);
}

