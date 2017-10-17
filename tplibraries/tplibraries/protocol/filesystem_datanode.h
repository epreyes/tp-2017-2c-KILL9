/*
 * filesystem_datanode.h
 *
 *  Created on: 14/10/2017
 *      Author: utnso
 */

#ifndef TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_
#define TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_



typedef struct {
    int idNodo;
    int idBloque;
    char* contenido;
} t_escribirDataNode;


#define	SUCCESS	    1
#define GET_BLOQUE  2
#define ACCESOAPROBADO  3
#define ACCESODENEGADO  1

#endif /* TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_ */
