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
	long finByte;
	char* contenido;
} t_escribirBloque;

typedef struct __attribute__((packed)){
	int idOperacion;
	int idBloque;
	long finByte;
	char* contenido;
} t_leerBloque;


#define	SUCCESS	    	1
#define GET_BLOQUE  	2
#define SET_BLOQUE  	3
#define ACCESOAPROBADO  4
#define ACCESODENEGADO  5

#define NODE_HSK 	6		//Envio DataNode
#define HSK_OK   	7		//Respuesta FS
#define SET_BLOQUE_OK 	8
#define GET_BLOQUE_OK   9
#define YAMA_HSK       10

/**GET BLOQUE **/
t_leerBloque* recibirPaquete(int fd);

#endif /* TPLIBRARIES_PROTOCOL_FILESYSTEM_DATANODE_H_ */
