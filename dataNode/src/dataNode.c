/*
 ============================================================================
 Name        : dataNode.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "header/dataNode.h"

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <tplibraries/sockets/socket.h>
#include <tplibraries/struct/struct.h>
#include <tplibraries/protocol/filesystem_datanode.h>
#include "header/connectionDataNodeFileSystem.h"
#include "header/const.h"

int fd_server_socket;

int main(void) {
	load_data_node_properties_files();

	connectionToFileSystem();
	return EXIT_SUCCESS;
}

/// ftruncate(fd, sizeof(t_directorio) * MAX_DIR_FS); ver despues
