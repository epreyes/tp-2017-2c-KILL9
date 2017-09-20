/*
 * connectionDataNodeFileSystem.h
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */


#ifndef HEADERS_CONNECTIONDATANODEFILESYSTEM_H_
#define HEADERS_CONNECTIONDATANODEFILESYSTEM_H_

 const char * mapped_data_node;

/** Funciones */

void process_request_file_system(int client_socket);

void connectionToFileSystem();

void get_block(int client_socket);

void set_block(int client_socket);

int map_data_node(void);

#endif /* HEADERS_CONNECTIONDATANODEFILESYSTEM_H_ */
