/*
 * fs_consola.c
 *
 *  Created on: 20/10/2017
 *      Author: utnso
 */

#include "fs_consola.h"

void ejecutarConsola() {

	char comandos[100];
	char instruccionConsola[100];
	char param1[100];
	char param2[100];
	char param3[2];
	printf("FS Yama\nTipear ayuda para ver los comandos disponibles\n");
	while (1) {
		printf(">");
		fgets(comandos, 100, stdin);
		sscanf(comandos, "%s %s %s %s", instruccionConsola, param1, param2,
				param3);

		if (vieneDeNoEstado == 1)
			if (!estaFormateado()
					&& (strcmp(instruccionConsola, FORMATEAR) != 0
							&& strcmp(instruccionConsola, AYUDA) != 0)) {
				printf("Debe formatear antes de comenzar a usar el nuevo fs\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}

		if (strcmp(instruccionConsola, FORMATEAR) == 0) {
			int formato = formatear();
			if (formato == 0)
				printf("Formato completo\n");
			else
				printf("Error en el formateo \n");

			instruccionConsola[0] = '\0';
			param1[0] = '\0';
			param2[0] = '\0';
			continue;
		}

		if (strcmp(instruccionConsola, INFOARCHIVO) == 0) {

			int estadoArchivo = obtenerEstadoArchivo(param1);
			switch (estadoArchivo) {
			case 0:
				printf("Online\n");
				break;
			case -1:
				printf("No existe el archivo\n");
				break;
			case -2:
				printf("Offline\n");
				break;
			}

			instruccionConsola[0] = '\0';
			param1[0] = '\0';
			param2[0] = '\0';
			continue;
		}

		if (strcmp(instruccionConsola, ESTADONODOS) == 0) {

			imprimirEstadoNodos();

			instruccionConsola[0] = '\0';
			param1[0] = '\0';
			param2[0] = '\0';
			continue;
		}

		if (strcmp(instruccionConsola, CREAR_DIRECTORIO) == 0) {

			if (param1[0] == '\0') {
				printf("Falta parametro directorio\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}

			int res = crearDirectorio(param1);
			if (res == -1) {
				printf("El directorio ya existe\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}
			if (res == FS_SIN_FORMATO) {
				printf("Fs sin formato, ejecute el comando formatear\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}
			if (res == 0) {
				printf("Directorio creado\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;

			}

		}

		if (strcmp(instruccionConsola, AYUDA) == 0) {
			printf("Comandos disponibles\n");
			printf("--------------------\n");
			printf(
					"cpfrom [PathArchivoOrigenFsNativo] [PathDirDestinoFsYama] [-b/-t]\n");
			printf(
					"-Copia un archivo del fs nativo a un directorio del fs yama. Por defecto copia en binario si no se especifica tipo\n");
			printf("mkdir [directorio]\n");
			printf("-Crea un directorio\n");
			printf("ls [path]\n");
			printf("-Lista los archivos del path indicado\n");
			printf("formatear\n");
			printf("-Da formato al filesystem\n");
			printf("cpto [PathArchivoOrigenYama] [PathDirDestinoFsNativo]\n");
			printf("Copia un archivo de yama a un directorio del fs nativo\n");
			printf("cat [PathArchivoYama]\n");
			printf("Muestra el contenido de un archivo por consola\n");
		}

		// Nuevos comandos
		if (strcmp(instruccionConsola, LISTAR_ARCHIVOS) == 0) {

			if (!existeDirectorio(param1)) {
				printf("No existe el directorio\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}

			t_list* l = list_create();
			l = listarArchivos(param1);

			if (list_size(l) == 0) {
				printf("Directorio vacio\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				list_destroy(l);
				continue;
			}

			int j = 0;
			for (j = 0; j < list_size(l); j++) {
				printf("%s\n", list_get(l, j));
			}

			list_destroy(l);
		}

		if (strcmp(instruccionConsola, LEERARCHIVO) == 0) {
			char* result = leerArchivo(param1);

			if (result == NULL )
				printf("No existe el archivo\n");
			else {
				printf("%s\n", result);
				free(result);
			}
		}

		if (strcmp(instruccionConsola, ESCRIBIR_ARCHIVO_LOCAL_YAMA) == 0) {

			int fd;
			struct stat sbuf;
			char* archivo = param1;

			if ((fd = open(archivo, O_RDWR)) == -1) {
				printf("No existe el archivo %s en el fs nativo\n", param1);
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}

			if (stat(archivo, &sbuf) == -1) {
				perror("stat");
				exit(1);
			}

			char* lect = mmap((caddr_t) 0, sbuf.st_size, PROT_READ, MAP_SHARED,
					fd, 0);

			if (lect == NULL ) {
				perror("error en map\n");
				exit(1);
			}

			char* dirArchivo = obtenerDirArchivo(param1);

			char* destino = string_new();

			string_append(&destino, param2);
			string_append(&destino, "/");
			string_append(&destino, obtenerNombreArchivo(dirArchivo));

			int escribir;
			if (strncmp(param3, "-b", 2) == 0) {
				escribir = escribirArchivo(destino, lect, BINARIO,
						sbuf.st_size);
			}
			if (strncmp(param3, "-t", 2) == 0)
				escribir = escribirArchivo(destino, lect, TEXTO, 0);

			if (strncmp(param3, "-t", 2) != 0 && strncmp(param3, "-b", 2) != 0) {
				escribir = escribirArchivo(destino, lect, BINARIO,
						sbuf.st_size);
			}

			if (escribir == 0) {
				log_info(logger, "Escritura de %s realizada con exito", param1);
				printf("Escritura ok\n");
			} else {
				switch (escribir) {
				case SIN_ESPACIO:
					printf(
							"No se pudo escribir el archivo, no hay espacio en disco\n");
					log_error(logger,
							"No se pudo escribir el archivo, no hay espacio en disco");
					break;
				case ARCHIVO_EXISTENTE:
					printf(
							"No se pudo escribir el archivo, el archivo ya existe\n");
					log_error(logger,
							"No se pudo escribir el archivo, el archivo ya existe");
					break;

				case NO_HAY_NODOS:
					printf("No hay nodos conectados para la escritura\n");
					break;

				case TAMANIO_RENGLON_INVALIDO:
					printf("Tamanio de renglon es mayor al del bloque\n");
					log_error(logger,
							"Tamanio de renglon es mayor al del bloque");
					break;

				case NO_EXISTE_DIR_DESTINO:
					printf("El directorio destino no existe\n");
					log_error(logger, "El directorio destino no existe");
					break;
				}

			}

		}

		if (strcmp(instruccionConsola, COPIARDEYAMAALOCAL) == 0) {
			int resultado = copiarDesdeYamaALocal(param1, param2);
			if (resultado == -1)
				printf("No existe el archivo\n");
			if (resultado == -2)
				printf("No existe el directorio destino\n");
			if (resultado == 0)
				printf("Escritura en local ok\n");
		}

		instruccionConsola[0] = '\0';
		param1[0] = '\0';
		param2[0] = '\0';

	}
}

