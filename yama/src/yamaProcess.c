/*
 * yamaProcess.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */

#include "yamaProcess.h"
#include "yamaFS.h"
#include "yamaOperations.h"

int processRequest(Yama* yama, void* request, int master, int nbytes) {
	log_trace(yama->log, "Processing Master Request...");

	t_header head;
	memcpy(&head, request, sizeof(t_header));

	void* payload = malloc(head.msg_size);
	free(request);
	recv(master, payload, head.msg_size, 0);

	return getFileSystemInfo(yama, head, payload, master);
}

int processOperation(Yama* yama, int master, t_header head, void* fsInfo) {
	int result;

	switch (head.op) {
	case 'T':
		result = transformation(yama, head, master, fsInfo);
		break;
	case 'L':
		result = localReduction(yama, head, master, fsInfo);
		break;
	case 'G':
		result = globalReduction(yama, head, master, fsInfo);
		break;
	case 'S':
		result = finalStore(yama, head, master, fsInfo);
	}
	return result;
}
