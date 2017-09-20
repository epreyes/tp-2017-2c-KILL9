/*
 ============================================================================
 Name        : dataNode.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include "headers/dataNode.h"
#include <connections/socket.h>
#include "headers/connectionDataNodeFileSystem.h"
#include <struct/struct.h>
int fd_server_socket;

int main(void) {
	load_data_node_properties_files();

	connectionToFileSystem();
	return EXIT_SUCCESS;
}


//TODO mover a otro archivo de funciones
void  load_data_node_properties_files(){
	t_data_node = config_create(DATA_NODE_PATH);
	data_node_config = malloc(sizeof(t_data_node_config));
	data_node_config->file_system_ip= config_get_string_value(t_data_node,"IP_FILESYSTEM");
	data_node_config->file_system_port= config_get_string_value(t_data_node,"PORT_FILESYSTEM");
	data_node_config->path_data_bin= config_get_string_value(t_data_node,"PATH_DATABIN");

	free(t_data_node);
}
