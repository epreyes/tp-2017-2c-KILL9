/*
 * dataNode.h
 *
 *  Created on: 10/9/2017
 *      Author: utnso
 */

#ifndef DATANODE_DATANODE_H_
#define DATANODE_DATANODE_H_
#include <commons/config.h>
/*
 * STATIC
 */
static char* IP_FILESYSTEM = "IP_FILESYSTEM";
static char* PORT_FILESYSTEM="PORT_FILESYSTEM";
static char* NAME_NODO="NAME_NODO";
static char* PORT_WORKER="PORT_WORKER";
static char* PORT_DATANODE="PORT_DATANODE";
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


t_data_node_config* data_node_conf;
void load_properties_files();


#endif /* DATANODE_DATANODE_H_ */
