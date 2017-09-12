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

int main(void) {
	load_properties_files();

	return EXIT_SUCCESS;
}

void  load_properties_files(){
	t_data_node_config = config_create(DATA_NODE_PATH);
}
