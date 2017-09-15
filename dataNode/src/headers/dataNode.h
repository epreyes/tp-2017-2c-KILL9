/*
 * dataNode.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#ifndef DATANODE_DATANODE_H_
#define DATANODE_DATANODE_H_
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <connections/socket.h>
#include <connections/handler.h>
#include <commons/config.h>
#include <commons/log.h>
#include "protocols/protocol.h"
/*
 * STATIC
 */
static char* PATH_DATABIN="PATH_DATABIN";

static char* DATA_NODE_PATH="./properties/dataNode.properties";
t_config * t_data_node;

/**
 * Struct
 */
typedef struct{
 char * file_system_ip;
 char * file_system_port;
}t_data_node_config;


/**Variables Globales */
t_data_node_config* data_node_conf;
t_log* infoLogger;
int32_t fileSystemSocket;

void load_properties_files();


#endif /* DATANODE_DATANODE_H_ */
