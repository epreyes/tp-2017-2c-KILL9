/*
 * connectionDataNodeFileSystem.h
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */


#ifndef HEADERS_CONNECTIONDATANODEFILESYSTEM_H_
#define HEADERS_CONNECTIONDATANODEFILESYSTEM_H_
#define DATANODE 1

//int block_size = 1048576; /**Erro al incluir aca **
void * mapped_data_node;
/** Funciones */

void process_request_file_system(int client_socket);

void connectionToFileSystem();

void get_block(int client_socket);

void set_block(int client_socket);

#endif /* HEADERS_CONNECTIONDATANODEFILESYSTEM_H_ */
