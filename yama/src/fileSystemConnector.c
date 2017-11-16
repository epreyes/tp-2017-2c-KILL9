/*
 * yamaFS.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */
#include "headers/fileSystemConnector.h"

void sendErrorToFileSystem(char* name) {
	/*configuro la ip del filesystem*/
	char fs_ip[16];
	strcpy(fs_ip, config_get_string_value(yama->config, "FS_IP"));
	/*configuro el puerto del filesystem*/
	int fs_port = config_get_int_value(yama->config, "FS_PUERTO");
	void* fsInfo = NULL;

	/*me conecto al filesystem*/
	log_info(yama->log, "Conectando al filesystem %s:%d ...", fs_ip, fs_port);
	Client fs_client = connectClient(fs_ip, fs_port);
	if (fs_client.socket_id > -1) {
		log_info(yama->log, "La conexion con el filesystem fue exitosa!");
		/*creo el buffer que contendra la solicitud al filesystem*/
		name = string_substring_from(name, 8);
		int buffersize = sizeof(int) + sizeof(char) + sizeof(int) + strlen(name)
				+ 1;
		int sizename = strlen(name) + 1;

		void* buffer = malloc(buffersize);
		int handShake = 10;
		memcpy(buffer, &handShake, sizeof(int));
		memcpy(buffer + sizeof(int), "E", sizeof(char));
		memcpy(buffer + sizeof(int) + sizeof(char), &sizename, sizeof(int));
		memcpy(buffer + sizeof(int) + sizeof(char) + sizeof(int), name,
				sizename);

		if (send(fs_client.socket_server_id, buffer, buffersize, 0) > 0) {
			log_trace(yama->log,
					"Se envió notificación de error en job al filesystem, para desbloquear archivo.");
		} else {
			log_error(yama->log,
					"Error al enviar notificación de fallo al filesystem.");
		}
	} else {
		log_error(yama->log, "No se puede conectar al filesystem %s:%d.", fs_ip,
				fs_port);
	}
	disconnectClient(&fs_client);
}

void* getFileSystemInfo(char* name) {

	/*configuro la ip del filesystem*/
	char fs_ip[16];
	strcpy(fs_ip, config_get_string_value(yama->config, "FS_IP"));
	/*configuro el puerto del filesystem*/
	int fs_port = config_get_int_value(yama->config, "FS_PUERTO");
	void* fsInfo = NULL;

	/*me conecto al filesystem*/
	log_info(yama->log, "Conectando al filesystem %s:%d ...", fs_ip, fs_port);
	Client fs_client = connectClient(fs_ip, fs_port);

	if (fs_client.socket_id > -1) {
		log_info(yama->log, "La conexion con el filesystem fue exitosa!");

		/*creo el buffer que contendra la solicitud al filesystem*/

		name = string_substring_from(name, 8);

		int buffersize = sizeof(int) + sizeof(char) + sizeof(int) + strlen(name)
				+ 1;

		int sizename = strlen(name) + 1;

		void* buffer = malloc(buffersize);
		int handShake = 10;
		memcpy(buffer, &handShake, sizeof(int));
		memcpy(buffer + sizeof(int), "T", sizeof(char));
		memcpy(buffer + sizeof(int) + sizeof(char), &sizename, sizeof(int));
		memcpy(buffer + sizeof(int) + sizeof(char) + sizeof(int), name,
				sizename);

		int infoSize = 0;
		int bytesSend = send(fs_client.socket_server_id, buffer, buffersize, 0);
		if (bytesSend > 0) {
			free(buffer);

			char confirm;
			recv(fs_client.socket_server_id, &confirm, sizeof(char),
			MSG_WAITALL);

			if (confirm == 'O') {
				int blocks;
				int recved = recv(fs_client.socket_server_id, &blocks,
						sizeof(int),
						MSG_WAITALL);

				if (recved > 0) {
					infoSize = blocks * sizeof(block_info);

					buffer = malloc(infoSize);
					if (recv(fs_client.socket_server_id, buffer, infoSize,
					MSG_WAITALL) > 0) {
						fsInfo = malloc(sizeof(int) + infoSize);
						memcpy(fsInfo, &infoSize, sizeof(int));
						memcpy(fsInfo + sizeof(int), buffer, infoSize);
						free(buffer);
						addToNodeList(fsInfo);
					} else {
						log_error(yama->log,
								"Error ecibing payload from FileSystem...");
						fsInfo = malloc(sizeof(char));
						memcpy(fsInfo, "E", sizeof(char));
					}

				}
			} else {
				log_warning(yama->log, "No existe el archivo solicitado.");
				fsInfo = malloc(sizeof(char));
				memcpy(fsInfo, "E", sizeof(char));
			}
		} else {
			log_error(yama->log, "No se puede conectar al filesystem %s:%d.");
			fsInfo = malloc(sizeof(char));
			memcpy(fsInfo, "E", sizeof(char));
		}
	} else {
		log_error(yama->log, "No se puede conectar al filesystem %s:%d.", fs_ip,
				fs_port);
		fsInfo = malloc(sizeof(char));
		memcpy(fsInfo, "E", sizeof(char));
	}

	disconnectClient(&fs_client);
	return fsInfo;
}

int findFile(char* fileName) {
	int index = 0;
	if (!list_is_empty(yama->tabla_info_archivos)) {
		for (index = 0; index < list_size(yama->tabla_info_archivos); index++) {
			elem_info_archivo* fileInfo = list_get(yama->tabla_info_archivos,
					index);
			if (strcmp(fileName, fileInfo->filename) == 0) {
				return index;
			}
		}
	}

	return -1;
}

elem_info_archivo* getFileInfo(int master, t_job* job) {
	elem_info_archivo* info = NULL;

	/*recivo del master el tamanio del nombre del archivo.*/
	void* buff = malloc(sizeof(int));
	recv(master, buff, sizeof(int), 0);

	int nameSize;
	memcpy(&nameSize, buff, sizeof(int));
	free(buff);

	/*recivo los bytes correspondientes al tamanio del nombre del archivo*/
	buff = malloc(nameSize);
	recv(master, buff, nameSize, MSG_WAITALL);

	char fileName[nameSize];
	strncpy(fileName, buff, nameSize);
	fileName[nameSize] = '\0';
	free(buff);

	log_info(yama->log, "Solicitando informacion del archivo: %s. Job %d.",
			fileName, job->id);

	int fileIndex = findFile(fileName);

	/*Si ya tengo la info del archivo, en la lista, la saco de ahi.*/
	if (fileIndex >= 0) {
		log_info(yama->log, "Obteniendo informacion de cache. Job %d.",
				job->id);
		elem_info_archivo* fileInfo = list_get(yama->tabla_info_archivos,
				fileIndex);
		info = fileInfo;
		return info;
	}
	/*Si no tengo la informacion en la tabla, se la pido al filesystem*/
	else {
		log_info(yama->log, "Obteniendo informacion del filesystem. Job %d.",
				job->id);
		void* fsInfo = getFileSystemInfo(fileName);

		if ((*(char*) fsInfo) == 'E') {
			return fsInfo;
		} else {
			elem_info_archivo* fileInfo = malloc(sizeof(elem_info_archivo));
			strcpy(fileInfo->filename, fileName);

			int size;
			memcpy(&size, fsInfo, sizeof(int));
			fileInfo->sizeInfo = size;

			fileInfo->info = malloc(size);
			memcpy(fileInfo->info, fsInfo + sizeof(int), size);

			fileInfo->blocks = size / sizeof(block_info);

			list_add(yama->tabla_info_archivos, fileInfo);

			info = fileInfo;
			return info;
		}
	}
}
