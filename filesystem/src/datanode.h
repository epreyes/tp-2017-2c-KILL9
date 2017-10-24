/*
 * datanode.h
 *
 *  Created on: 19/10/2017
 *      Author: utnso
 */

#ifndef DATANODE_H_
#define DATANODE_H_

#include <stddef.h>
#include <commons/log.h>
#include <tplibraries/protocol/filesystem_datanode.h>

#define DATANODE_DUMMY 0

// Pedidos al datanode
int escribirEnDataNode(int idBloque, char* contenido, int socketNodo,
		long finBytes, t_log* logger);
char* leerDeDataNode(int idBloque, int socketNodo, long finBytes, t_log* logger);

#endif /* DATANODE_H_ */
