/*
 * fs_core.c
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#include "fs_core.h"

t_directorio* inicioTablaDirectorios;

// Obtiene el ultimo indice de directorio valido

int obtenerUltimoIndiceDirValido() {
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

int crearDirectorio(char* nombreDir) {

	if (!estaFormateado())
		return FS_SIN_FORMATO;

	if (existeDirectorio(nombreDir))
		return -1;

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

bool existeDirectorio(char* path) {

	char** directorios = NULL;

	char *p = path;
	int d = 0;
	while (*p) {
		if (*p == '/')
			d++;

		p++;
	}

	bool esDirRaiz = (d == 0);

	if (esDirRaiz) {
		t_directorio* dir = (t_directorio*) inicioTablaDirectorios;
		int j = 0;
		for (j = 0; j < 100; j++) {
			if (strcmp(dir->nombre, path) == 0) {
				return true;
			}

			dir++;
		}

		return false;

	}

	// No esta en raiz
	directorios = string_split(path, "/");

	bool primerDir = false;

	int indiceAnt = 0;
	int j = 0;

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
				//break;
			}

			dir++;
		}
	}

	bool ok = (resultados == d + 1);
	if (ok)
		return true;
	else
		return false;

}

// Formatea el FS

void formatear() {
// TODO: debe formatearse tambien los bitmaps y eliminarse los archivos csv (por ahora hay un script que lo hace por fuera)

	log_info(logger, "Comenzando formateo de disco");

	int fd;
	struct stat sbuf;
	char* archivo;

	archivo = fs->m_directorios;

	log_info(logger, "Formateando archivo de directorios %s...", archivo);

	if ((fd = open(archivo, O_RDWR)) == -1) {
		log_error(logger, "No existe el archivo %s", archivo);
		exit(1);
	}

	ftruncate(fd, sizeof(t_directorio) * MAX_DIR_FS);

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	inicioTablaDirectorios = mmap((caddr_t) 0, sbuf.st_size,
	PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (inicioTablaDirectorios == NULL) {
		perror("error en map\n");
		exit(1);
	}

	int i = 0;
	t_directorio* dir = inicioTablaDirectorios;

	dir->indice = 0;
	dir->padre = -1;
	dir++;

	for (i = 1; i < MAX_DIR_FS; i++) {
		dir->indice = i;
		dir->padre = -1;
		memset(dir->nombre, '\0', 256);
		dir++;
	}

	log_info(logger, "Formateado de disco ok");

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

// Dado un path devuelve la lista de archivos

t_list* listarArchivos(char* path) {

	int indiceDir = obtenerIndiceDir(path);

	if (indiceDir == -1)
		return NULL;

	t_list* tablaArchivos = list_create();
	DIR *d;
	struct dirent *dir;

	char *dpath = string_new();

	string_append(&dpath, fs->m_archivos);
	string_append(&dpath, string_itoa(indiceDir));

	d = opendir(dpath);
	if (d) {
		while ((dir = readdir(d)) != 0) {
			if (strcmp(dir->d_name, ".") != 0
					&& strcmp(dir->d_name, "..") != 0) {

				char *dest = malloc(strlen(dir->d_name));
				strncpy(dest, dir->d_name, strlen(dir->d_name));
				dest[strlen(dir->d_name)] = '\0';

				list_add(tablaArchivos, dest);

			}

		}

		closedir(d);
	}

	free(dpath);

	return tablaArchivos;

}

// Crea el bitmap para la gestion de bloques de un nodo
// TODO: queda pendiente ver el tema de inicializacion, por ahora pisa el archivo si ya existe

void crearBitMapBloquesNodo(t_nodo* nodo) {

	int fd, offset;
	char *data;
	struct stat sbuf;
	char* archivo = string_new();

	string_append(&archivo, fs->m_bitmap);
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

	ftruncate(fd, (nodo->total) / 8);

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	bloquesBitMap = mmap((caddr_t) 0, sbuf.st_size, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);

	if (bloquesBitMap == NULL) {
		perror("error en map\n");
		exit(1);
	}

	t_bitarray* b = bitarray_create(bloquesBitMap, nodo->total);

	t_nodosBitMap* nbm = malloc(sizeof(t_nodosBitMap));

	nbm->idNodo = nodo->id;
	nbm->bitMapBloques = b;

	list_add(nodosBitMap, nbm);

	free(archivo);

}

// Dado un id de nodo, obtiene el bitmap de gestion de bloques
t_bitarray* obtenerBitMapBloquesNodo(t_nodo* nodo) {

	int fd, offset;
	char *data;
	struct stat sbuf;
	char* archivo = string_new();

	string_append(&archivo, fs->m_bitmap);

	string_append(&archivo, string_itoa(nodo->id));
	string_append(&archivo, ".dat");

	char* bloquesBitMap;

// Si no existe lo creo
	if ((fd = open(archivo, O_RDWR)) == -1) {
		log_error(logger, "No existe el archivo de gestion de bloques %s",
				archivo);
		exit(1);
	}

	if (stat(archivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

	bloquesBitMap = mmap((caddr_t) 0, sbuf.st_size, PROT_READ, MAP_SHARED, fd,
			0);

	if (bloquesBitMap == NULL) {
		perror("error en map\n");
		exit(1);
	}

	t_bitarray* b = bitarray_create(bloquesBitMap, nodo->total);

	free(archivo);

	return b;

}

// Valida si existe un archivo dado por parametro en un directorio dado
// Asume que el directorio del path existe
int existeArchivo(char* path) {

	char* dirArchivo = obtenerDirArchivo(path);

	int c = obtenerProfDir(path);

	t_list* archivos = listarArchivos(dirArchivo);

// El directorio esta vacio
	if (list_size(archivos) == 0)
		return -1;

// El directorio tiene archivos

	char* archivosDir = string_new();
	archivosDir = obtenerDirectorios(path)[c];

	int j = 0;

	for (j = 0; j < list_size(archivos); j++) {

		int t = string_length((char*) list_get(archivos, j));
		log_debug(logger, "Comparando %s con %s (%d)", archivosDir,
				(char*) list_get(archivos, j), t);

		if (strncmp(archivosDir, (char*) list_get(archivos, j), t - 5) == 0)
			return 0;

	}

	return -1;

}

// Obtiene la info del archivo en base al archivo de metadata

t_archivoInfo* obtenerArchivoInfo(char* path) {

	if (!estaFormateado())
		return NULL;

	t_archivoInfo* tInfo = malloc(sizeof(t_archivoInfo));

	if (existeArchivo(path) == -1) {
		log_debug(logger, "No existe el archivo %s", path);
		return NULL;
	}

	char* dirArchivo = obtenerDirArchivo(path);
	int indiceDir = obtenerIndiceDir(dirArchivo);

	char *dirMetadata = string_new();

	string_append(&dirMetadata, fs->m_archivos);
	string_append(&dirMetadata, string_itoa(indiceDir));
	string_append(&dirMetadata, "/");
	string_append(&dirMetadata, obtenerNombreArchivo(path));
	string_append(&dirMetadata, ".csv");

	t_config * metadata = config_create(dirMetadata);

	if (metadata == NULL) {
		log_error(logger, "No se pudo abrir la metadata de archivo %s",
				dirMetadata);
		return NULL;
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

	if (arch == NULL) {
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

bool ordenarPorCarga(t_nodo *nodoConMasCarga, t_nodo *nodoConMenosCarga) {
	return nodoConMasCarga->libre > nodoConMenosCarga->libre;
}

// Decide cual nodo elegir para copiar el bloque, debe ser el que este con menos carga de bloques usados
t_list* decidirNodo(t_list* nodos) {
	list_sort(nodos, (void*) ordenarPorCarga);
	return nodos;
}

// Actualiza el bitmap de bloques por la escritura de un archivo
void actualizarBitMapBloques(t_list* bloquesInfo) {

	int i = 0;
	int j = 0;

	for (j = 0; j < list_size(bloquesInfo); j++) {
		t_bloqueInfo* bi = list_get(bloquesInfo, j);

		for (i = 0; i < list_size(nodosBitMap); i++) {
			t_nodosBitMap* nbm = list_get(nodosBitMap, i);
			// TODO: ver si hacer el id de nodo numerico o string
			if (nbm->idNodo == atoi(bi->idNodo0)) {
				// Seteo el bit de bloque ocupado
				if (!bitarray_test_bit(nbm->bitMapBloques, bi->idBloque0)) {

					bitarray_set_bit(nbm->bitMapBloques, bi->idBloque0);
					log_debug(logger, "Seteando bloque de bitmap %d de nodo %d",
							bi->idBloque0, nbm->idNodo);
				}
			}
			if (nbm->idNodo == atoi(bi->idNodo1)) {
				// Seteo el bit de bloque ocupado
				if (!bitarray_test_bit(nbm->bitMapBloques, bi->idBloque1)) {

					bitarray_set_bit(nbm->bitMapBloques, bi->idBloque1);
					log_debug(logger,
							"Seteando bloque de bitmap (copia) %d de nodo %d",
							bi->idBloque1, nbm->idNodo);
				}
			}

		}
	}

}

// Crea el archivo de metadata de un archivo recien escrito
int crearArchivoMetadata(t_list* bloquesInfo, char* path, int tipo, int tamanio) {

	int fd, offset;
	struct stat sbuf;

	char* archivoMetadata;

	int siz = strlen("TAMANIO=") + strlen(string_itoa(tamanio)) + strlen("\n");

	if (tipo == BINARIO)
		siz += strlen("TIPO=BINARIO\n");

	if (tipo == TEXTO)

		siz += strlen("TIPO=TEXTO\n");

	int i = 0;

	for (i = 0; i < list_size(bloquesInfo); i++) {
		t_bloqueInfo* bi = list_get(bloquesInfo, i);

		siz += strlen("BLOQUE");
		siz += strlen(string_itoa(i));
		siz += strlen("COPIA0=[");
		siz += strlen(bi->idNodo0);
		siz += strlen(",");
		siz += strlen(string_itoa(bi->idBloque0));
		siz += strlen("]\n");
		siz += strlen("BLOQUE");
		siz += strlen(string_itoa(i));
		siz += strlen("COPIA1=[");
		siz += strlen(bi->idNodo1);
		siz += strlen(",");
		siz += strlen(string_itoa(bi->idBloque1));
		siz += strlen("]\n");
		siz += strlen("BLOQUE");
		siz += strlen(string_itoa(i));
		siz += strlen("BYTES=");
		siz += strlen(string_itoa(bi->finBytes));
		siz += strlen("\n");

	}

	char* metadata = malloc(siz);

	siz = 8;

// Armo el contenido del archivo
	offset = 0;

	memcpy(metadata + offset, "TAMANIO=", siz);
	offset += siz;

	siz = strlen(string_itoa(tamanio));

	memcpy(metadata + offset, string_itoa(tamanio), siz);
	offset += siz;

	siz = strlen("\n");

	memcpy(metadata + offset, "\n", siz);
	offset += siz;

	if (tipo == BINARIO) {

		siz = strlen("TIPO=BINARIO\n");

		memcpy(metadata + offset, "TIPO=BINARIO\n", siz);
		offset += siz;
	}

	if (tipo == TEXTO) {

		siz = strlen("TIPO=TEXTO\n");

		memcpy(metadata + offset, "TIPO=TEXTO\n", siz);
		offset += siz;

	}

	i = 0;
	for (i = 0; i < list_size(bloquesInfo); i++) {
		t_bloqueInfo* bi = list_get(bloquesInfo, i);

		siz = strlen("BLOQUE");

		memcpy(metadata + offset, "BLOQUE", siz);
		offset += siz;

		siz = strlen(string_itoa(i));

		memcpy(metadata + offset, string_itoa(i), siz);
		offset += siz;

		siz = strlen("COPIA0=[");
		memcpy(metadata + offset, "COPIA0=[", siz);
		offset += siz;

		siz = strlen(bi->idNodo0);
		memcpy(metadata + offset, bi->idNodo0, siz);
		offset += siz;
		siz = strlen(",");
		memcpy(metadata + offset, ",", strlen(","));
		offset += siz;
		siz = strlen(string_itoa(bi->idBloque0));
		memcpy(metadata + offset, string_itoa(bi->idBloque0), siz);
		offset += strlen(string_itoa(bi->idBloque0));
		siz = strlen("]\n");
		memcpy(metadata + offset, "]\n", siz);
		offset += siz;

		siz = strlen("BLOQUE");
		memcpy(metadata + offset, "BLOQUE", siz);
		offset += siz;
		siz = strlen(string_itoa(i));
		memcpy(metadata + offset, string_itoa(i), siz);
		offset += siz;
		siz = strlen("COPIA1=[");
		memcpy(metadata + offset, "COPIA1=[", siz);
		offset += siz;
		siz = strlen(bi->idNodo1);
		memcpy(metadata + offset, bi->idNodo1, siz);
		offset += siz;
		siz = strlen(",");
		memcpy(metadata + offset, ",", siz);
		offset += siz;
		siz = strlen(string_itoa(bi->idBloque1));
		memcpy(metadata + offset, string_itoa(bi->idBloque1), siz);
		offset += siz;
		siz = strlen("]\n");
		memcpy(metadata + offset, "]\n", siz);
		offset += siz;

		siz = strlen("BLOQUE");
		memcpy(metadata + offset, "BLOQUE", siz);
		offset += siz;
		siz = strlen(string_itoa(i));
		memcpy(metadata + offset, string_itoa(i), siz);
		offset += siz;

		siz = strlen("BYTES=");
		memcpy(metadata + offset, "BYTES=", siz);
		offset += siz;
		siz = strlen(string_itoa(bi->finBytes));
		memcpy(metadata + offset, string_itoa(bi->finBytes), siz);
		offset += siz;
		siz = strlen("\n");
		memcpy(metadata + offset, "\n", siz);
		offset += siz;

	}

	char* farchivo = string_new();

	string_append(&farchivo, fs->m_archivos);

	char* dirArchivo = obtenerDirArchivo(path);

	string_append(&farchivo, string_itoa(obtenerIndiceDir(dirArchivo)));

	struct stat st = { 0 };

// Si no existe el directorio del indice, lo creo
	if (stat(farchivo, &st) == -1) {
		mkdir(farchivo, 0700);
	}

	string_append(&farchivo, "/");
	string_append(&farchivo, obtenerNombreArchivo(path));
	string_append(&farchivo, ".csv");

// Creo el archivo de metadata
	if ((fd = open(farchivo, O_RDWR | O_CREAT, 00777)) == -1) {
		log_error(logger, "No se pudo abrir el archivo: %s", farchivo);
		exit(1);
	}

// Trunco con el tamanio obtenido
	ftruncate(fd, offset);

	if (stat(farchivo, &sbuf) == -1) {
		perror("stat");
		exit(1);
	}

// mapeo a memoria
	archivoMetadata = mmap((caddr_t) 0, sbuf.st_size, PROT_READ | PROT_WRITE,
	MAP_SHARED, fd, 0);

	if (archivoMetadata == NULL) {
		perror("error en map\n");
		exit(1);
	}

// Escribo metadata a archivo
	memcpy(archivoMetadata, metadata, offset);

	free(farchivo);

	return 0;

}

// Devuelve la lista de bloques necesarios para el archivo
int obtenerBloquesNecesarios(char* contenido, int tipo) {

	int offset = 0;

	int d = 0; // tamanio del renglon actual
	int restanteBloque = TAMANIO_BLOQUE;
	int offsetbloque = 0;

	char* p = contenido;
	int i = 0; // Comienzo desde el bloque 0

	if (contenido == NULL)
		return -1;

	if (tipo == TEXTO) {

		for (p = contenido + offset; *p; p++) {
			d++;

			if (*p == '\n') {

				if (d > TAMANIO_BLOQUE) {
					log_error(logger,
							"Tamanio de renglon es mayor al del bloque");
					exit(1);
				}

				// El renglon actual entra en el espacio del bloque disponible
				if (d <= restanteBloque) {

					;

				} else
				// nuevo bloque
				{

					restanteBloque = TAMANIO_BLOQUE;
					i++;

					offsetbloque = 0;

				}

				restanteBloque -= d;
				offsetbloque += d;
				offset += d;
				d = 0;

			}
		}
	}

	if (tipo == BINARIO) {
// TODO
		return 0;
	}

	return i + 1;

}

// Dado una cantidad de bloques necesarios, recorre todos los nodos conectados y devuelve los id de bloques junto a los nodos asociados
// Los devuelve balanceados. Si la funcion de escritura falla por algun motivo en los nodos (que no sea faltante de bloques) debe rollbackearse las estructuras administrativas
// Si no hay bloques disponibles para la escritura del archivo, devuelve nulo.

// TODO: la copia no debe estar en el mismo nodo

t_list* obtenerBloquesLibres(int cantBloques) {

	t_list* res = list_create();
	t_list* t = list_create();

	int nod = 0;

	int j = 0;
	for (j = 0; j < cantBloques; j++) {

		t = decidirNodo(nodos->nodos);
		t_nodo* nodo = list_get(t, nod);

// TODO: reemplazar esta linea con la busqueda en nodosBitMap
		t_bitarray* bitMapBloque = obtenerBitMapBloquesNodo(nodo);

		if (bitMapBloque == NULL) {
			log_error(logger, "No se pudo abrir el bitmap de bloques");
			exit(1);
		}

		int i = 0;
		t_idNodoBloque* nb = malloc(sizeof(t_idNodoBloque));
		nb->idNodo = -1;
		for (i = 0; i < bitMapBloque->size; i++) {

			bool estaEnAuxiliar(t_idNodoBloque* nb) {
				return i == nb->idBloque && nodo->id == nb->idNodo;
			}

			t_idNodoBloque* registro = list_find(res, (void*) estaEnAuxiliar);

			if (!bitarray_test_bit(bitMapBloque, i) && registro == NULL) {

				nb = malloc(sizeof(t_idNodoBloque));
				nb->idBloque = i;
				nb->idNodo = nodo->id;
				log_debug(logger, "Bloque: %d - Nodo: %d", i, nodo->id);
				nodo->libre--;

				list_add(res, nb);
				break;
			}
		}

	}

	if (list_size(res) != cantBloques) {
// No alcanza, deshacer cambios hechos en nodos tomando la lista res

		int j = 0;
		int i = 0;
		for (j = 0; j < list_size(res); j++) {
			t_idNodoBloque* nb = list_get(res, j);
			for (i = 0; i < list_size(nodos->nodos); i++) {
				t_nodo* nodo = list_get(nodos->nodos, i);
				if (nodo->id == nb->idNodo) {
					log_debug(logger, "Rollbackeando bloque %d de nodo %d...",
							nb->idBloque, nodo->id);
					nodo->libre++;

					break;
				}
			}
		}

		return NULL;
	}

	return res;

}

int leerArchivo(char* path) {

	return 0;

}

bool estaFormateado() {

	t_directorio* dir = inicioTablaDirectorios;

	if (dir == 0xffffffffffffffff)
		return false;
	else {

		return true;
	}

	return false;

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
