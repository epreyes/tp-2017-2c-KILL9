/*
 * finalStore.c
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#include "headers/finalStore.h"

void finalStorage(){
	fs_node_rq datos;
	log_info(logger,"Master %d: Obteniendo datos de almacenado Final",socket_master);

//OBTENIENDO DATOS DE MASTER
	readBuffer(socket_master,24,&(datos.rg_tmp));
	readBuffer(socket_master,sizeof(int),&(datos.fileNameSize));
	datos.fileName=malloc(datos.fileNameSize);
	readBuffer(socket_master,datos.fileNameSize,datos.fileName);

	log_info(logger,"Master %d: Datos de almacenamiento final obtenidos",socket_master);

//GENERO PAQUETE PARA FS
	char* fileContent;
	fileContent = serializeFile(datos.rg_tmp);

	fs_rq* fs_datos;
	fs_datos = malloc(sizeof(fs_rq));

	//EMPAQUETO
	fs_datos->operation = 'S';							//CODIGO
	fs_datos->fileNameSize = datos.fileNameSize;		//NOMBRE DEL ARCHIVO FINAL
	fs_datos->fileName = malloc(fs_datos->fileNameSize);
	strcpy(fs_datos->fileName, datos.fileName);
	fs_datos->fileSize = strlen(fileContent)+1;			//CONTENIDO DEL ARCHIVO
	fs_datos->file = malloc(fs_datos->fileSize);
	strcpy(fs_datos->file, fileContent);

	void* buffer;
	int bufferSize=sizeof(char)+sizeof(int)*2+fs_datos->fileNameSize+fs_datos->fileSize;
	buffer = malloc(bufferSize);

	//SERIALIZO
	memcpy(buffer,&(fs_datos->operation),sizeof(char));
	memcpy(buffer+sizeof(char),&(fs_datos->fileNameSize),sizeof(int));
	memcpy(buffer+sizeof(char)+sizeof(int),fs_datos->fileName,fs_datos->fileNameSize);
	memcpy(buffer+sizeof(char)+sizeof(int)+fs_datos->fileNameSize, &(fs_datos->fileSize), sizeof(int));
	memcpy(buffer+sizeof(char)+sizeof(int)*2+fs_datos->fileNameSize, fs_datos->file, fs_datos->fileSize);

	//ENVIO
	send(socket_filesystem,buffer,bufferSize,0);

	free(fs_datos);
	free(buffer);

//ESPERO RESPUESTA DE FS
	char rs_fs;

	readBuffer(socket_filesystem,sizeof(char),&rs_fs);
	if(rs_fs=='O'){
		log_info(logger, "Master %d: Archivo almacenado", socket_master);
	}else{
		log_error(logger, "Master %d: Error al intentar almacenar el archivo", socket_master);
		exit(1);
	}

//RESPUESTA A MASTER
	typedef struct{
		int		runTime;
		char	result;
	}fs_node_rs;

	fs_node_rs* answer = malloc(sizeof(fs_node_rs));
	answer->result = rs_fs;
	answer->runTime = 12;

	void* buff = malloc(sizeof(int)+sizeof(char));

	memcpy(buff,&(answer->runTime),sizeof(int));
	memcpy(buff+sizeof(int),&(answer->result),sizeof(char));

	free(answer);
	free(buff);
	log_trace(logger, "Master %d: Almacenado Finalizado", socket_master);

}
