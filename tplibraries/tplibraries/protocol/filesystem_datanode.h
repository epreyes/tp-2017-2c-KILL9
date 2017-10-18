/*
 * filesystem_datanode.h
 *
 *  Created on: 14/10/2017
 *      Author: utnso
 */

#ifndef TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_
#define TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_

typedef struct __attribute__((packed)){
	int idOperacion;
	int idBloque;
    int finByte;
	char* contenido;
} t_escribirBloque;

typedef struct __attribute__((packed)){
	int idOperacion;
	int idBloque;
    int finByte;
	char* contenido;
} t_leerBloque;


#define	SUCCESS	    	1
#define GET_BLOQUE  	2
#define SET_BLOQUE  	3
#define ACCESOAPROBADO  3
#define ACCESODENEGADO  1

#define NODE_HSK 		1		//Envio DataNode
#define HSK_OK   		1		//Respuesta FS
#define SET_BLOQUE_OK 	1

#endif /* TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_ */
