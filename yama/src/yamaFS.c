/*
 * yamaFS.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */
#include "yamaFS.h"

void* getFileSystemInfo(Yama* yama, t_header head, void* payload) {
	char fs_ip[15];
	strcpy(fs_ip, config_get_string_value(yama->config, "FS_IP"));
	int fs_port = config_get_int_value(yama->config, "FS_PUERTO");

	Client fs_client = connectClient(fs_ip, fs_port);
	char* name = (char*) payload;

	void* fsInfo = NULL;

	void* buffer = malloc(sizeof(t_header) + head.msg_size);
	memcpy(buffer, &head, sizeof(t_header));
	memcpy(buffer + sizeof(t_header), name, head.msg_size);

	if (send(fs_client.socket_server_id, buffer,
			sizeof(t_header) + head.msg_size, 0) > 0) {

		free(buffer);

		buffer = malloc(sizeof(t_header));

		int recved = recv(fs_client.socket_server_id, buffer, sizeof(t_header),
				0);
		if (recved > 0) {
			t_header headFromFS = *(t_header*) buffer;
			free(buffer);
			buffer = malloc(headFromFS.msg_size);
			if( recv(fs_client.socket_server_id, buffer, headFromFS.msg_size, 0) > 0){
				fsInfo = malloc(sizeof(t_header)+headFromFS.msg_size);
				memcpy(fsInfo, &headFromFS, sizeof(t_header));
				memcpy(fsInfo+sizeof(t_header), buffer, headFromFS.msg_size);
			}else{
				perror("Recibing payload from FileSystem...");
			}

		} else {
			perror("Recibing...");
		}
	} else {
		perror("Sending");
	}
	disconnectClient(&fs_client);
	updateNodeList(yama, fsInfo);
	viewNodeTable(yama);
	return fsInfo;
}

int findFile(Yama* yama, char* fileName) {
	int index = 0;
	if ( !list_is_empty(yama->file_info) ) {
		for (index = 0; index < list_size(yama->file_info); index++) {
			t_fileInfo* fileInfo = list_get(yama->file_info, index);
			if (strcmp(fileName, fileInfo->filename) == 0) {
				return index;
			}
		}
	}

	return -1;
}

t_fileInfo* getFileInfo(Yama* yama, t_header head, char* fileName, int master) {
	t_fileInfo* info = NULL;
	int fileIndex = findFile(yama, (char*) fileName);

	if (fileIndex >= 0) {
		t_fileInfo* fileInfo = list_get(yama->file_info, fileIndex);
		info = fileInfo;
	} else {
		void* fsInfo = getFileSystemInfo(yama, head, fileName);

		t_fileInfo* fileInfo =  malloc(sizeof(t_fileInfo));
		fileInfo->filename = malloc(strlen(fileName));
		memcpy(fileInfo->filename, fileName, strlen(fileName));

		t_header* head = malloc(sizeof(t_header));
		memcpy(head, fsInfo, sizeof(t_header));
		fileInfo->sizeInfo = head->msg_size;

		fileInfo->info = malloc(head->msg_size);
		memcpy( fileInfo->info, fsInfo + sizeof(t_header), head->msg_size );

		fileInfo->blocks = head->msg_size / sizeof(block);

		list_add(yama->file_info, fileInfo);
		info = fileInfo;
	}
	return info;
}
