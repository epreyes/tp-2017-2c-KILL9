/*
 * yamaFS.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */
#include "headers/fileSystemConnector.h"

void* getFileSystemInfo(char* name) {

	/*configuro la ip del filesystem*/
	char fs_ip[15];
	strcpy(fs_ip, config_get_string_value(yama->config, "FS_IP"));
	/*configuro el puerto del filesystem*/
	int fs_port = config_get_int_value(yama->config, "FS_PUERTO");

	/*me conecto al filesystem*/
	Client fs_client = connectClient(fs_ip, fs_port);

	log_info(yama->log, "Succesfull FileSystem connection.");
	void* fsInfo = NULL;

	/*creo el buffer que contendra la solicitud al filesystem*/

	name = string_substring_from(name, 8);

	int buffersize = sizeof(int) + sizeof(char) + sizeof(int) + strlen(name);


	int sizename = strlen(name);

	void* buffer = malloc(buffersize);
	int handShake = 10;
	memcpy(buffer, &handShake, sizeof(int));
	memcpy(buffer + sizeof(int), "T", sizeof(char));
	memcpy(buffer + sizeof(int) + sizeof(char), &sizename, sizeof(int));
	memcpy(buffer + sizeof(int) + sizeof(char) + sizeof(int), name, sizename);

	int infoSize = 0;
	int bytesSend = send(fs_client.socket_server_id, buffer, buffersize, 0);
	if (bytesSend > 0) {
		free(buffer);

		char confirm;
		recv(fs_client.socket_server_id, &confirm, sizeof(char), MSG_WAITALL);

		if (confirm == 'O') {
			int blocks;
			int recved = recv(fs_client.socket_server_id, &blocks, sizeof(int),
					MSG_WAITALL);

			if (recved > 0) {
				infoSize = blocks*sizeof(block_info);

				buffer = malloc(infoSize);
				if (recv(fs_client.socket_server_id, buffer, infoSize,
						MSG_WAITALL) > 0) {
					fsInfo = malloc(sizeof(int) + infoSize);
					memcpy(fsInfo, &infoSize, sizeof(int));
					memcpy(fsInfo + sizeof(int), buffer, infoSize);
				} else {
					log_error(yama->log, "Error ecibing payload from FileSystem...");
				}

			}
		} else {
			log_error(yama->log, "File requested does not exist.");
		}
	} else {
		log_error(yama->log, "Error in filesystem connection.");
	}
	disconnectClient(&fs_client);
	addToNodeList(fsInfo);

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

elem_info_archivo* getFileInfo(int master) {
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

	log_info(yama->log, "Solicitando informacion del archivo: %s. Job %d.", fileName,
			yama->jobs + master);

	int fileIndex = findFile(fileName);

	/*Si ya tengo la info del archivo, en la lista, la saco de ahi.*/
	if (fileIndex >= 0) {
		log_info(yama->log, "Obteniendo informacion de cache. Job %d.", yama->jobs + master);
		elem_info_archivo* fileInfo = list_get(yama->tabla_info_archivos,
				fileIndex);
		info = fileInfo;
	}
	/*Si no tengo la informacion en la tabla, se la pido al filesystem*/
	else {
		log_info(yama->log, "Obteniendo informacion del filesystem. Job %d.", yama->jobs + master);
		void* fsInfo = getFileSystemInfo(fileName);
		elem_info_archivo* fileInfo = malloc(sizeof(elem_info_archivo));
		strcpy(fileInfo->filename, fileName);

		int* size = malloc(sizeof(int));
		memcpy(size, fsInfo, sizeof(int));
		fileInfo->sizeInfo = *size;

		fileInfo->info = malloc(*size);
		memcpy(fileInfo->info, fsInfo + sizeof(int), *size);

		fileInfo->blocks = *size / sizeof(block_info);

		list_add(yama->tabla_info_archivos, fileInfo);

		info = fileInfo;
	}
	return info;
}
