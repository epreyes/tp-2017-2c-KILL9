/*
 * struct.h
 *
 *  Created on: 15/9/2017
 *      Author: utnso
 */

#ifndef STRUCT_STRUCT_H_
#define STRUCT_STRUCT_H_

/**
 * Structs
 */

typedef struct {
	char* file_system_port;
	char* file_sytem_ip;
}__attribute__ ((__packed__)) t_file_system_config;


typedef struct{
 char* file_system_ip;
 char* file_system_port;
 char* data_node_port;
 char* 	path_data_bin;
}__attribute__ ((__packed__)) t_data_node_config;


#endif /* STRUCT_STRUCT_H_ */
