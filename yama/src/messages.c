/*
 * messages.c
 *
 *  Created on: 10/11/2017
 *      Author: utnso
 */

#include "headers/messages.h"

char* masterConnectedMsg(int master) {
	char* msg;
	asprintf(&msg, "Master %d connected!", master);
	return msg;
}

char* masterDisconnectedMsg(int master) {
	char* msg;
	asprintf(&msg, "Master %d disconnected!", master);
	return msg;
}

char* processOperationMsg(int master, int op) {
	char* msg;
	switch (op) {
	case 'T':
		asprintf(&msg, "Request of Transformation. Master %d.", master);
		break;
	case 'L':
		asprintf(&msg, "Request of Reduction. Master %d.", master);
		break;
	case 'G':
		asprintf(&msg, "Request of Global Reduction. Master %d", master);
		break;
	case 'S':
		asprintf(&msg, "Request of Final Store. Master %d.", master);
		break;
	}

	return msg;
}

char* startTransformationMsg(int master) {
	char* msg;
	asprintf(&msg, "Starting Transformation process. Master %d.", master);
	return msg;
}

char* gettingFileInfoMsg(char* fileName, int master) {
	char* msg;
	asprintf(&msg, "Getting info of file: %s. Master %d.", fileName, master);
	return msg;
}

char* gettingFileInfoFromMsg(char* from, int master) {
	char* msg;
	asprintf(&msg, "Getting info from %s. Master %d.", from, master);
	return msg;
}

char* sendResponseMsg(int master, int bytes, void* response) {
	char op;
	memcpy(&op, response, sizeof(char));

	char* msg;
	switch (op) {
	case 'T':
		asprintf(&msg, "Transformation Response: sent %d bytes to Master %d.", bytes, master);
		break;
	case 'L':
		asprintf(&msg, "Local Reduction Response: sent %d bytes to Master %d.", bytes, master);
		break;
	case 'G':
		asprintf(&msg, "Global Reduction: sent %d bytes to Master %d.", bytes, master);
		break;
	case 'S':
		asprintf(&msg, "Final Store Response: sent %d bytes to Master %d.", bytes, master);
		break;
	}

	return msg;

}
