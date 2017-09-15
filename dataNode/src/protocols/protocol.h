/*
 * protocol.h
 *
 *  Created on: 14/9/2017
 *      Author: utnso
 */

#ifndef PROTOCOLS_PROTOCOL_H_
#define PROTOCOLS_PROTOCOL_H_
#include <stdio.h>
// TODO mover a tp_share_library -> error al compilar
// si se importa desde tp_share_library
typedef enum PROTOCOL_DATANODE_TO_FILESYSTEM{
	HANDSHAKE_CONNECTION_DATANODE_TO_FILESYSTEM,
	DATA_ENUM
}PROTOCOL_DATANODE_TO_FILESYSTEM;


typedef enum PROTOCOL_FILESYSTEM_TO_DATANODE {
	HANDSHAKE_CONNECTION_FILESYSTEM_TO_DATANODE_OK,
}PROTOCOL_FILESYSTEM_TO_DATANODE;

#endif /* PROTOCOLS_PROTOCOL_H_ */
