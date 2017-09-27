/*
 * yamaFS.c
 *
 *  Created on: 21/9/2017
 *      Author: utnso
 */
#include "yamaFS.h"

int getFileSystemInfo(Yama* yama, t_header head, void* payload, int master) {
	int response = 0;
	Client fs_client = connectClient(yama->fs_ip, yama->fs_port);
	t_header header = head;
	char* name = (char*) payload;

	void* buffer = malloc(sizeof(t_header) + head.msg_size);

	memcpy(buffer, &header, sizeof(t_header));
	memcpy(buffer + sizeof(t_header), name, head.msg_size);

	if (send(fs_client.socket_server_id, buffer,
			sizeof(t_header) + head.msg_size, 0) > 0) {
		free(buffer);
		buffer = malloc(sizeof(t_header));

		int recved = recv(fs_client.socket_server_id, buffer, sizeof(t_header),
				0);
		if (recved > 0) {
			t_header headRecived = *(t_header*) buffer;

			free(buffer);
			buffer = malloc(headRecived.msg_size);
			recv(fs_client.socket_server_id, buffer, headRecived.msg_size, 0);

			//doPlanning(buffer);

			response = processOperation(yama, master, headRecived, buffer);
		} else {
			perror("Recibing...");
			response = 0;
		}
	} else {
		perror("Sending");
		response = 0;
	}
	disconnectClient(&fs_client);
	return response;
}
