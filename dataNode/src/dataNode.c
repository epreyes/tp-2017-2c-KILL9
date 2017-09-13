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
#include "dataNode.h"
#include <tp-share-library/socket.h>

int fd_server_socket;

int main(void) {
	load_properties_files();

	fd_server_socket = connect_to_socket(data_node_conf->file_system_ip,data_node_conf->file_system_port);



	return EXIT_SUCCESS;
}


//TODO mover a otro archivo de funciones
void  load_properties_files(){
	t_data_node = config_create(DATA_NODE_PATH);
	data_node_conf = malloc(sizeof(t_data_node_config));
	data_node_conf->file_system_ip = config_get_string_value(t_data_node,"IP_FILESYSTEM");
	data_node_conf->file_system_port= config_get_string_value(t_data_node,"PORT_FILESYSTEM");
	free(t_data_node);
}
