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
#include <tplibraries/sockets/socket.h>
#include <commons/config.h>
#include <commons/log.h>
#include <tplibraries/struct/struct.h>
#include <tplibraries/protocol/filesystem_datanode.h>

// Definiciones
#define LOG_INFO "logInfo"
#define LOG_ERROR "logError"

//typedef enum {
//	ACCESOAPROBADO = 0,
//	ACCESODENEGADO = 1
//}dataNode;
/*
 * STATIC
 */

typedef struct __attribute__((packed)) {
	 int 	handshake;
	 int  	block_quantity;
	 int 	id_nodo;
	 int 	data_size;
	 char*  worker_ip_port;
} t_infoNodo;

static char* DATA_NODE_PATH="../properties/dataNode.properties";

/** Variables globales **/
t_config * t_data_node;
t_data_node_config* data_node_config;


t_log* infoLogger;
t_log* logError;

int32_t fileSystemSocket;

void load_data_node_properties_files(void);


#endif /* DATANODE_DATANODE_H_ */
