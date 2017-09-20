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
#include <struct/struct.h>

/*
 * STATIC
 */

static char* DATA_NODE_PATH="./properties/dataNode.properties";

/** Variables globales **/
t_config * t_data_node;
t_data_node_config* data_node_config;


t_log* infoLogger;
int32_t fileSystemSocket;

void load_data_node_properties_files(void);


#endif /* DATANODE_DATANODE_H_ */
