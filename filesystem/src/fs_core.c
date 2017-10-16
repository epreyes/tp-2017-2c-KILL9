/*
 * fs_core.c
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#include "fs_core.h"

// TODO: parametrizar esto
char* direccionBitMap = "/home/proyectomacro/SO/fs/bin/metadata/bitmaps/nodo";
char* direccionArchivos = "/home/proyectomacro/SO/fs/bin/metadata/archivos/";

t_directorio* inicioTablaDirectorios;

// Obtiene el ultimo indice de directorio valido

int obtenerUltimoIndiceDirValido() {
	t_directorio* dir = inicioTablaDirectorios;
	dir++; // Salteo raiz

	while (dir != NULL ) {
		if (dir->padre == -1)
			return dir->indice;

		dir++;
	}

	return -1;

}

// Dado un path crea el directorio indicado

int crearDirectorio(char* nombreDir) {

	// TODO: validar existencia

	// Obtengo el directorio padre
	int directorioPadre = obtenerIndiceDirPadre(nombreDir);

	// Obtengo un indice valido
	int indice = obtenerUltimoIndiceDirValido();

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

	free(directorios);

	return 0;

}

// Formatea el FS

void formatear() {
	// 1. escribir sizeof(t_directorio)*100 en el archivo de directorios

	// TODO: debe formatearse tambien los bitmaps

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

int obtenerIndiceDirPadre(char* path) {

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

int obtenerIndiceDir(char* path) {

	char** directorios = NULL;

	char *p = path;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	bool esDirRaiz = (d == 0);
	int j = 0;

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

	// Archivo en raiz
	if (esDirRaiz && strcmp(path, "") == 0)
		return 0;

	// No esta en raiz
	directorios = string_split(path, "/");

	bool primerDir = false;

	int indiceAnt = 0;

	int k = 0;
	int resultados = 0;

	j = 0;

	for (k = 0; k < d + 1; k++) {

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

	bool ok = (resultados == d + 1);
	if (ok)
		return indiceAnt;
	else
		return -1;

}

// Dado un path devuelve la lista de archivos. Consultar si debe devolver tambien directorios (probablemente si)

t_list* listarArchivos(char* path) {

	int indiceDir = obtenerIndiceDir(path);

	if (indiceDir == -1)
		return -1;

	t_list* tablaArchivos = list_create();
	DIR *d;
	struct dirent *dir;

	char *dpath = string_new();

	string_append(&dpath, direccionArchivos);
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

	free(dpath);

	return tablaArchivos;

}

// Crea el bitmap para la gestion de bloques de un nodo. Si el archivo ya existe, sobreescribe su bitmap (Esto ultimo no esta bien, sirve para pruebas)

void crearBitMapBloquesNodo(t_nodo* nodo) {

	int fd, offset;
	char *data;
	struct stat sbuf;
	char* archivo = string_new();

	string_append(&archivo, direccionBitMap);

	string_append(&archivo, string_itoa(nodo->id));
	string_append(&archivo, ".dat");

	char* bloquesBitMap;

	int existe = 1;

	// Si no existe lo creo
	if ((fd = open(archivo, O_RDWR)) == -1) {
		existe = 0;
		if ((fd = open(archivo, O_RDWR | O_CREAT, 00700)) == -1) {
			exit(1);
		}
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	bloquesBitMap = mmap((caddr_t) 0, sbuf.st_size, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, 0);

	if (bloquesBitMap == NULL ) {
		perror("error en map\n");
		exit(1);
	}

	if (existe == 0) {
		t_bitarray* b = bitarray_create(bloquesBitMap, nodo->total);
	}

	free(archivo);

}

// Dado un id de nodo, obtiene el bitmap de gestion de bloques
t_bitarray* obtenerBitMapBloquesNodo(t_nodo* nodo) {

	int fd, offset;
	char *data;
	struct stat sbuf;
	char* archivo = string_new();

	string_append(&archivo, direccionBitMap);

	string_append(&archivo, string_itoa(nodo->id));
	string_append(&archivo, ".dat");

	char* bloquesBitMap;

	// Si no existe lo creo
	if ((fd = open(archivo, O_RDWR)) == -1) {
		exit(1);
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	bloquesBitMap = mmap((caddr_t) 0, sbuf.st_size, PROT_READ, MAP_SHARED, fd,
			0);

	if (bloquesBitMap == NULL ) {
		perror("error en map\n");
		exit(1);
	}

	t_bitarray* b = bitarray_create(bloquesBitMap, nodo->total);

	free(archivo);

	return b;

}

// Valida si existe un archivo dado por parametro
int existeArchivo(char* path) {

	char* dirArchivo = obtenerDirArchivo(path);

	int c = obtenerProfDir(path);

	t_list* archivos = listarArchivos(dirArchivo);

	char* archivosDir = string_new();
	archivosDir = obtenerDirectorios(path)[c];

	// TODO: consultar ESTO NO FUNCA en la 2da invocacion (por algun motivo raro)
	/*bool estaEnAuxiliar(char* buscar) {
	 return strcmp(buscar, archivosDir) == 0;
	 }

	 dirBuscar = list_find(archivos, (void*) estaEnAuxiliar);*/

	int j = 0;

	for (j = 0; j < list_size(archivos); j++) {

		int t=string_length((char*) list_get(archivos, j));


		if (strcmp(archivosDir, string_substring_until((char*) list_get(archivos, j),t-4)) == 0)
			return 0;
	}

	return -1;

}

// Obtiene la info del archivo en base al archivo de metadata

t_archivoInfo* obtenerArchivoInfo(char* path) {

	t_archivoInfo* tInfo = malloc(sizeof(t_archivoInfo));

	if (existeArchivo(path) == -1)
		return NULL ;

	char* dirArchivo = obtenerDirArchivo(path);
	int indiceDir = obtenerIndiceDir(dirArchivo);

	char *dirMetadata = string_new();

	string_append(&dirMetadata, direccionArchivos);
	string_append(&dirMetadata, string_itoa(indiceDir));
	string_append(&dirMetadata, "/");
	string_append(&dirMetadata, obtenerNombreArchivo(path));
	string_append(&dirMetadata, ".csv");

	t_config * metadata = config_create(dirMetadata);

	if (metadata == NULL ) {
		return NULL ;
	}

	if (config_has_property(metadata, "TAMANIO")) {
		tInfo->tamanio = 0;
		tInfo->tamanio = config_get_int_value(metadata, "TAMANIO");
	} else {
		tInfo->tamanio = -1;
	}
	if (config_has_property(metadata, "TIPO")) {
		tInfo->tipo = 0;
		tInfo->tipo = config_get_int_value(metadata, "TIPO");
	} else {
		tInfo->tipo = -1;
	}

	int fd, offset;
	char *data;
	struct stat sbuf;
	char* archivo;

	if ((fd = open(dirMetadata, O_RDWR)) == -1) {
		//log_error(logger, "No existe el archivo %s", archivo);
		exit(1);
	}

	if (stat(dirMetadata, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	char* arch = mmap((caddr_t) 0, sbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);

	if (arch == NULL ) {
		perror("error en map\n");
		exit(1);
	}

	char* p = arch;
	int slinea = 0;

	for (p = arch; *p; p++) {

		if (*p == '\n')
			slinea++;
	}

	close(fd);

	// Salteo los dos primeros registros
	slinea -= 2;

	// Calculo la cantidad de bloques teniendo en cuenta que son 3 atributos por bloque
	int cantBloques = slinea / 3;

	if (slinea % 3 > 0) {
		perror("Error en metadata");
		exit(1);
	}

	tInfo->bloques = list_create();

	int j = 0;
	for (j = 0; j < cantBloques; j++) {

		t_bloqueInfo* bi = malloc(sizeof(t_bloqueInfo));

		char* bloqueInfo = string_new();
		string_append(&bloqueInfo, "BLOQUE");
		string_append(&bloqueInfo, string_itoa(j));
		string_append(&bloqueInfo, "COPIA0");

		if (config_has_property(metadata, bloqueInfo)) {
			bi->idNodo0 = obtenerIdNodo(
					config_get_string_value(metadata, bloqueInfo));
			bi->idBloque0 = obtenerIdBloque(
					config_get_string_value(metadata, bloqueInfo));
		}

		free(bloqueInfo);

		bloqueInfo = string_new();
		string_append(&bloqueInfo, "BLOQUE");
		string_append(&bloqueInfo, string_itoa(j));
		string_append(&bloqueInfo, "COPIA1");

		if (config_has_property(metadata, bloqueInfo)) {
			bi->idNodo1 = obtenerIdNodo(
					config_get_string_value(metadata, bloqueInfo));
			bi->idBloque1 = obtenerIdBloque(
					config_get_string_value(metadata, bloqueInfo));
		}

		free(bloqueInfo);

		bloqueInfo = string_new();
		string_append(&bloqueInfo, "BLOQUE");
		string_append(&bloqueInfo, string_itoa(j));
		string_append(&bloqueInfo, "BYTES");

		if (config_has_property(metadata, bloqueInfo))
			bi->finBytes = config_get_int_value(metadata, bloqueInfo);

		free(bloqueInfo);

		list_add(tInfo->bloques, bi);

	}

	free(dirMetadata);
	config_destroy(metadata);

	return tInfo;

}

// Dado un archivo y un contenido, escribe en el fs. Los bloques del archivo deben distribuirse por todos los nodos conectados
// Esta primera version solo escribe en un nodo (toma el primer nodo conectado y registrado), tampoco balancea la carga y solo hace una copia
// de los bloques. Faltan validaciones tambien.

int escribirArchivo(char* path, char* contenido, int tipo) {

	// 1. Verificar si hay espacio (lo chequeo del nodos.bin). La escritura deben actualizar este archivo

	// 2. Obtengo los bloques a escribir (debe balancearse)

	t_nodo* nodo = list_get(nodos->nodos, 0);

	// 3. Hago setBloque al nodo

	log_info(logger, "Nodo seleccionado para escribir %d", nodo->id);

	int bloquesNecesarios = 2;

	t_list* bloques = obtenerBloquesLibres(nodo, bloquesNecesarios);

	// No hay bloques disponibles requeridos para la operacion actual
	if (bloques == NULL ) {
		list_destroy(bloques);
		return -1;
	}

	// Escribo en los bloques reservados
	int i = 0;

	char bloque[TAMANIO_BLOQUE];

	int offset = 0;

	for (i = 0; i < list_size(bloques); i++) {

		if (tipo == BINARIO) {
			memcpy(bloque, contenido + offset, TAMANIO_BLOQUE);
			// Pedido al datanode
			//escribirEnBloque(list_get(bloques, i), bloque);
			log_info(logger, "Escribiendo en bloque %d: %s",
					list_get(bloques, i), bloque);
			offset += TAMANIO_BLOQUE;
		}

		else {
			// Texto
			;

		}

	}

}

// Dado un nodo, obtiene los bloques libres pedidos
t_list* obtenerBloquesLibres(t_nodo* nodo, int cantBloques) {

	t_bitarray* bitMapBloque = obtenerBitMapBloquesNodo(nodo);
	t_list* bloquesLibres = list_create();

	int i = 0;
	for (i = 0; i < bitMapBloque->size; i++)
		if (!bitarray_test_bit(bitMapBloque, i))
			if (cantBloques > 0) {
				list_add(bloquesLibres, i);
				cantBloques--;
			} else
				break;

// No existen los bloques requeridos disponibles
	if (cantBloques > 0)
		return NULL ;

	return bloquesLibres;

}

// Funciones auxiliares

// Dado un path completo devuelve el path (directorio) del archivo
char* obtenerDirArchivo(char* path) {

	char* dirobj = string_new();

	int c = obtenerProfDir(path);
	if (c > 0) {
		int j = 0;
		for (j = 0; j < c; j++) {
			string_append(&dirobj, obtenerDirectorios(path)[j]);

			if (j < (c - 1))
				string_append(&dirobj, "/");
		}

	}

	return dirobj;

}

// Dado un path nativo devuelve el nombre de archivo referenciado
char* obtenerNombreArchivo(char* path) {
	int c = obtenerProfDir(path);

	char* archivosDir = string_new();
	archivosDir = obtenerDirectorios(path)[c];

	return archivosDir;

}

char* obtenerIdNodo(char* cadena) {
	char** propiedades = string_split(cadena, ",");
	return string_substring_from(propiedades[0], 1);
}

int obtenerIdBloque(char* cadena) {
	char** propiedades = string_split(cadena, ",");
	return atoi(string_split(propiedades[1], "]")[0]);
}
