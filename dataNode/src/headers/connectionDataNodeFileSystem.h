/*
 * connectionDataNodeFileSystem.h
 *
 *  Created on: 13/9/2017
 *      Author: utnso
 */


#ifndef HEADERS_CONNECTIONDATANODEFILESYSTEM_H_
#define HEADERS_CONNECTIONDATANODEFILESYSTEM_H_
#define DATANODE 1

<<<<<<< HEAD
void * mapped_data_node;
//int block_size = 1048576; /**Erro al incluir aca **
=======
const char * mapped_data_node;
const int block_size =1048576;
>>>>>>> ad0384cf86aa636bfad207f4c33202887bb7962d
/** Funciones */

void process_request_file_system(int client_socket);

void connectionToFileSystem();

void get_block(int client_socket);

void set_block(int client_socket);

int map_data_node(void);

#endif /* HEADERS_CONNECTIONDATANODEFILESYSTEM_H_ */
