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

	instruccionConsola[0] = '\0';
	param1[0] = '\0';
	param2[0] = '\0';
	param3[0] = '\0';

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

		// Comando de prueba
		if (strcmp(instruccionConsola, "copy") == 0) {
			strcpy(instruccionConsola, "cpfrom");
			strcpy(param1, "/home/utnso/w.txt");
			strcpy(param2, ".");
		}

		if (strcmp(instruccionConsola, "read") == 0) {
			strcpy(instruccionConsola, "cat");
			strcpy(param1, "w.txt");
		}

		if (strcmp(instruccionConsola, ELIMINAR) == 0) {

			if (strncmp(param1, "-d", 2) != 0) {

				int eliminar = eliminarArchivo(param1);

				switch (eliminar) {
				case 0:
					printf("Eliminacion de archivo ok\n");
					log_info(logger, "Eliminacion de archivo ok: %s", param1);
					break;
				case -1:
					printf("No existe el archivo a eliminar\n");
					log_error(logger, "No existe el archivo a eliminar: %s",
							param1);
					break;
				}

			}

			if (strncmp(param1, "-d", 2) == 0) {
				int eliminar = eliminarDirectorio(param2);

				switch (eliminar) {
				case 0:
					printf("Eliminacion de directorio ok\n");
					log_info(logger, "Eliminacion de directorio ok: %s",
							param2);
					break;
				case DIRECTORIO_NO_VACIO:
					printf("Directorio no vacio, no se pudo eliminar\n");
					log_error(logger,
							"Directorio no vacio, no se pudo eliminar: %s",
							param2);
					break;
				case NO_EXISTE_DIR_DESTINO:
					printf("No existe el directorio a eliminar\n");
					log_error(logger, "No existe el directorio a eliminar: %s",
							param2);
					break;
				}

			}

			param1[0] = '\0';
			param2[0] = '\0';

		}

		if (strcmp(instruccionConsola, INFOARCHIVO) == 0) {

			if (param1[0] == '\0') {
				printf("Falta parametro archivo\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				param2[0] = '\0';
				continue;
			}

			int estadoArchivo = obtenerEstadoArchivo(param1);
			t_archivoInfo* info;
			int cantBloques;
			int i = 0;
			switch (estadoArchivo) {
			case 0:
				info = obtenerArchivoInfo(param1);

				cantBloques = list_size(info->bloques);

				printf("%s: Online\n", param1);
				printf("Tamanio: %d bytes (%d bloques) (", info->tamanio,
						cantBloques);

				if (info->tipo == BINARIO)
					printf("BINARIO)\n");
				else
					printf("TEXTO)\n");

				int tamanio=0;
				for (i = 0; i < cantBloques; i++) {
					t_bloqueInfo* bi = list_get(info->bloques, i);
					printf(
							"Nro Bloque %d : Nodo %d - IdBloque %d - finBloque: %d\n",
							bi->nroBloque, atoi(bi->idNodo0), bi->idBloque0,
							bi->finBytes);
					printf("Nro Bloque %d (copia) : Nodo %d - IdBloque %d\n",
							bi->nroBloque, atoi(bi->idNodo1), bi->idBloque1,
							bi->finBytes);
					tamanio+=bi->finBytes;

				}
				printf("Suma de bloques: %d\n", tamanio);
				free(info);
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

		if (strcmp(instruccionConsola, MD5SUM) == 0) {

			if (param1[0] == '\0') {
				printf("Falta parametro archivo\n");
				instruccionConsola[0] = '\0';
				param1[0] = '\0';
				continue;
			}

			char* md5 = obtenerMd5(param1);

			if (md5 == NULL) {
				printf("No existe el archivo indicado por parametro\n");
			} else {
				printf("hash: %s\n", md5);

			}
			instruccionConsola[0] = '\0';
			param1[0] = '\0';

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
					"-Copia un archivo del fs nativo a un directorio del fs yama. Por defecto copia en texto si no se especifica tipo\n");
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
			printf("md5sum [PathArchivoYama]\n");
			printf("Obtiene el hash md5 del archivo indicado\n");
			printf("infoarchivo [PathArchivoYama]\n");
			printf("Obtiene la composicion de bloques y nodos del archivo\n");
			printf("rm [PathArchivoYama] | rm -d [PathDirectorioYama]\n");
			printf("Elimina un archivo o un directorio\n");

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

			void* eliminarItem(char* item) {
				free(item);
				return 0;
			}

			list_destroy_and_destroy_elements(l, eliminarItem);

		}

		if (strcmp(instruccionConsola, LEERARCHIVO) == 0) {
			int errorCode = 0;
			char* result = leerArchivo(param1, &errorCode);

			switch (errorCode) {
			case -1:
				printf("No existe el archivo\n");
				break;
			case -2:
				printf("Algun nodo se desconecto\n");
				break;
			case 0:
				printf("%s\n", result);
				free(result);
				break;
			}

			param1[0] = '\0';

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
				param3[0] = '\0';
				continue;
			}

			if (stat(archivo, &sbuf) == -1) {
				perror("stat");
				exit(1);
			}

			char* lect = mmap((caddr_t) 0, sbuf.st_size, PROT_READ, MAP_SHARED,
					fd, 0);

			if (lect == NULL) {
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

			if (strncmp(param3, "-t", 2) != 0
					&& strncmp(param3, "-b", 2) != 0) {
				escribir = escribirArchivo(destino, lect, TEXTO, 0);
			}

			if (escribir == 0) {
				log_info(logger, "Escritura de %s realizada con exito", param1);

				munmap(lect, sbuf.st_size);

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
				case ERROR_MISMO_NODO_COPIA:
					printf(
							"El bloque copia no debe estar en el mismo nodo que el original\n");
					log_error(logger,
							"El bloque copia no debe estar en el mismo nodo que el original");
					break;
				}

			}

			param1[0] = '\0';
			param2[0] = '\0';
			param3[0] = '\0';

		}

		if (strcmp(instruccionConsola, COPIARDEYAMAALOCAL) == 0) {
			int resultado = copiarDesdeYamaALocal(param1, param2);
			if (resultado == -1)
				printf("No existe el archivo\n");
			if (resultado == -2)
				printf("No existe el directorio destino\n");
			if (resultado == -3)
				printf("Algun nodo se desconecto\n");
			if (resultado == 0)
				printf("Escritura en local ok\n");

			param1[0] = '\0';
			param2[0] = '\0';
		}

		if (strcmp(instruccionConsola, COPIARDEYAMAALOCAL) == 0) {

			instruccionConsola[0] = '\0';
			param1[0] = '\0';
			param2[0] = '\0';

		}
	}

}
