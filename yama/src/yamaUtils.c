/*
 * yamaUtils.c
 *
 *  Created on: 21/09/2017
 *      Author: utnso
 */

#include "yamaUtils.h"
#include <tplibraries/utils/utils.h>

char* getTmpName(t_header* head, int blockId, int masterId) {
	char name[28];
	long timestamp = current_timestamp();
	snprintf(name, sizeof(name), "%s%ld-%c-M%03d-B%03d", "/tmp/", timestamp,
			head->op, masterId, blockId);

	return name;
}

bool compareTransformationBlocks(void* b1, void* b2) {
	bool response;
	if (((tr_datos*) b1)->nodo <= ((tr_datos*) b2)->nodo) {
		response = 1;
	} else {
		response = 0;
	}
	return response;
}

void* sortTransformationResponse(t_list* buffer, int size) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	void* sortedBuffer = malloc(size);

	int index = 0;
	for (index = 0; index < list_size(buffer); index++) {
		tr_datos* data = (tr_datos*) list_get(buffer, index);
		memcpy(sortedBuffer + (index * sizeof(tr_datos)), data,
				sizeof(tr_datos));
	}
	return sortedBuffer;
}

bool compareLocalReductionBlocks(void* b1, void* b2) {
	bool response;
	if (((tr_datos*) b1)->nodo <= ((tr_datos*) b2)->nodo) {
		response = 1;
	} else {
		response = 0;
	}
	return response;
}

void* sortLocalReductionResponse(t_list* buffer, int size) {
	bool (*comparator)(void*, void*);
	comparator = &compareTransformationBlocks;
	list_sort(buffer, comparator);

	void* sortedBuffer = malloc(size);

	int index = 0;
	for (index = 0; index < list_size(buffer); index++) {
		tr_datos* data = (tr_datos*) list_get(buffer, index);
		memcpy(sortedBuffer + (index * sizeof(tr_datos)), data,
				sizeof(tr_datos));
	}
	return sortedBuffer;
}

void* buildPackage(void* masterRS, t_header head) {
	t_header headSend = head;
	void* package = malloc(sizeof(t_header) + head.msg_size);
	memcpy(package, &headSend, sizeof(t_header));
	memcpy(package + sizeof(t_header), masterRS, head.msg_size);

	return package;
}

int sendPackage(Yama* yama, int master, void* package, int sizepack) {
	int bytesSent = send(master, package, sizepack, 0);
	if (bytesSent > 0) {
		char message[30];
		snprintf(message, sizeof(message), "%d bytes sent to master id %d",
				bytesSent, master);
		log_trace(yama->log, message);
	} else {
		perror("Send response to master.");
	}
	return bytesSent;
}
