/*
 * fs_core.c
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#include "fs_core.h"

// Obtiene el ultimo indice de directorio valido

int obtenerUltimoIndiceDirValido(t_directorio* inicioTablaDirectorios) {
	t_directorio* dir = inicioTablaDirectorios;
	dir++; // Salteo raiz

	while (dir != NULL) {
		if (dir->padre == -1)
			return dir->indice;

		dir++;
	}

	return -1;

}

// Dado un path crea el directorio indicado

int crearDirectorio(t_directorio* inicioTablaDirectorios, char* nombreDir) {

	// TODO: debe crear el directorio en el fs nativo (metadata\archivos\dirindice)

	// TODO: validar existencia

	// Obtengo el directorio padre
	int directorioPadre = obtenerIndiceDirPadre(nombreDir,
			inicioTablaDirectorios);

	// Obtengo un indice valido
	int indice = obtenerUltimoIndiceDirValido(inicioTablaDirectorios);

	if (indice == -1)
		return -1;

	char *p = nombreDir;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	char** directorios = string_split(nombreDir, "/");

	t_directorio* nuevoDir;
	nuevoDir = (t_directorio*) inicioTablaDirectorios;

	nuevoDir += indice;
	nuevoDir->indice = indice;
	strcpy(nuevoDir->nombre, directorios[d]);
	nuevoDir->padre = directorioPadre;

	return 0;

}

// Formatea el FS

void formatear(t_directorio* inicioTablaDirectorios) {
	// 1. escribir sizeof(t_directorio)*100 en el archivo de directorios

	// TODO: debe formatearse tambien los bitmaps (nodos quizas)

	int i = 0;
	t_directorio* dir = inicioTablaDirectorios;

	dir->indice = 0;
	dir->padre = -1;
	dir++;

	for (i = 1; i < 100; i++) {
		dir->indice = i;
		dir->padre = -1;
		memset(dir->nombre, '\0', 256);
		dir++;
	}

}

// Dado un path devuelve el indice del directorio padre

int obtenerIndiceDirPadre(char* path, t_directorio* inicioTablaDirectorios) {

	char** directorios = NULL;

	char *p = path;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	bool esDirRaiz = (d == 0);

	if (esDirRaiz)
		return 0;

	// No esta en raiz
	directorios = string_split(path, "/");

	bool primerDir = false;

	int indiceAnt = 0;
	int j = 0;

	int k = 0;
	int resultados = 0;

	j = 0;

	for (k = 0; k < d; k++) {

		t_directorio* dir = (t_directorio*) inicioTablaDirectorios;

		for (j = 0; j < 100; j++) {
			char tl[255];
			strcpy(tl, directorios[k]);
			if (strcmp(dir->nombre, tl) == 0 && dir->padre == indiceAnt) {
				indiceAnt = j;
				resultados++;
				break;
			}

			dir++;
		}
	}

	bool ok = (resultados == d);
	if (ok)
		return indiceAnt;
	else
		return -1;

}

// Dado un path devuelve su directorio indice

int obtenerIndiceDir(char* path, t_directorio* inicioTablaDirectorios) {

	char** directorios = NULL;

	char *p = path;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	bool esDirRaiz = (d == 0);
	int j=0;

	if (esDirRaiz) {

		t_directorio* dir = (t_directorio*) inicioTablaDirectorios;

		for (j = 0; j < 100; j++) {
			char tl[255];
			strcpy(tl, path);
			if (strcmp(dir->nombre, tl) == 0 && dir->padre == 0)
				return j;

			dir++;
		}
	}

	// No esta en raiz
	directorios = string_split(path, "/");

	bool primerDir = false;

	int indiceAnt = 0;

	int k = 0;
	int resultados = 0;

	j = 0;

	for (k = 0; k < d+1; k++) {

		t_directorio* dir = (t_directorio*) inicioTablaDirectorios;

		for (j = 0; j < 100; j++) {
			char tl[255];
			strcpy(tl, directorios[k]);
			if (strcmp(dir->nombre, tl) == 0 && dir->padre == indiceAnt) {
				indiceAnt = j;
				resultados++;
				break;
			}

			dir++;
		}
	}

	bool ok = (resultados == d+1);
	if (ok)
		return indiceAnt;
	else
		return -1;

}

// Dado un path devuelve la lista de archivos. Consultar si debe devolver tambien directorios (probablemente si)

t_list* listarArchivos(char* path, t_directorio* inicioTablaDirectorios) {

	int indiceDir = obtenerIndiceDir(path, inicioTablaDirectorios);

	if (indiceDir == -1)
		return -1;

	t_list* tablaArchivos = list_create();
	DIR *d;
	struct dirent *dir;

	char *dpath = string_new();
	string_append(&dpath, "/home/utnso/SO/fs/bin/metadata/archivos/"); //TODO: parametrizar esto
	//string_append(&dpath, "bin/metadata/archivos/"); //TODO: parametrizar esto
	string_append(&dpath, string_itoa(indiceDir));

	d = opendir(dpath);
	if (d) {
		while ((dir = readdir(d)) != 0) {
			if (strcmp(dir->d_name, ".") != 0
					&& strcmp(dir->d_name, "..") != 0) {

				list_add(tablaArchivos, dir->d_name);

			}

		}

		closedir(d);
	}
	return tablaArchivos;

}
