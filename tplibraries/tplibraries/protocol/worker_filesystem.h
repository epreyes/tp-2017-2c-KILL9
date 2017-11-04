/*
 * worker_yama.h
 *
 *  Created on: 26/10/2017
 *      Author: utnso
 */

#ifndef TPLIBRARIES_PROTOCOL_WORKER_FILESYSTEM_H_
#define TPLIBRARIES_PROTOCOL_WORKER_FILESYSTEM_H_

typedef struct{
	char	operation;			//'S' (save/storage)
	int		fileNameSize;		//Tamaño del nombre del archivo a almacenar
	char*	fileName;			//Nombre del archivo a almacenar
	int		fileSize;			//Tamaño total del archivo
	char*	file;				//Contenido del archivo
}fs_rq;

typedef struct{
	char	result;				//'E': error / 'O': ok
}fs_rs;

#define WORKER_HSK 11

#endif /* TPLIBRARIES_PROTOCOL_WORKER_FILESYSTEM_H_ */
