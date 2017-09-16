/*
 * yamaOperations.c
 *
 *  Created on: 14/09/2017
 *      Author: utnso
 */
#include "yamaOperations.h"

int getFileSystemInfo(Yama* yama, header head, int master) {
	Client fs_client = connectClient(yama->fs_ip, yama->fs_port);

	int fileNameSize = sizeof(head.file);
	char fileName[20];
	strcpy(fileName, head.file);

	void* buff = malloc(sizeof(int) + fileNameSize);
	memcpy(buff, &fileNameSize, sizeof(int));
	memcpy(buff + sizeof(int), fileName, fileNameSize);

	int bytesSent = send(fs_client.socket_server_id, buff, sizeof(int) + fileNameSize, 0);

	if (bytesSent > 0) {
		free(buff);
		buff = malloc(sizeof(int));
		recv(fs_client.socket_server_id, buff, sizeof(int), 0);
		int numBlocks = *(int*) buff;
		free(buff);
		buff = malloc(numBlocks * sizeof(block));
		recv(fs_client.socket_server_id, buff, numBlocks * sizeof(block), 0);

		processOperation(yama, head, master, buff);
	} else {

	}
	disconnectClient(&fs_client);
	return 1;
}

/*-----------------------------------------------------------------------------------*/
int transformation(Yama* yama, header head, int master, void* buff) {
	log_trace(yama->log, "Doing transformation...");

	return 1;
}

int localReduction(Yama* yama, header head, int master, void* buff) {
	log_trace(yama->log, "Doing Local Reduction...");
	return 1;
}

int globalReduction(Yama* yama, header head, int master, void* buff) {
	log_trace(yama->log, "Doing Global Reduction...");
	return 1;
}

int finalStore(Yama* yama, header head, int master, void* buff) {
	log_trace(yama->log, "Doing Final Store...");
	return 1;
}

int processFileSystemInfo(Yama* yama, block* response, int master, void* buff) {
	int result = 0;

	return result;
}

int processOperation(Yama* yama, header head, int master, void* buff) {

	int result;

	switch (head.op) {
	case T:
		result = transformation(yama, head, master, buff);
		break;
	case L:
		result = localReduction(yama, head, master, buff);
		break;
	case G:
		result = globalReduction(yama, head, master, buff);
		break;
	case S:
		result = finalStore(yama, head, master, buff);
	}
	return result;
}

