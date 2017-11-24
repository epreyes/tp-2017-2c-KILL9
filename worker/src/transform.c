/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"

void transformation(){
	tr_node datos;
	int i=0;
	log_info(logger,"Master %d: Obteniendo datos de transformación",socket_master);

//OBTENGO CONTENIDO DEL SCRIPT
	readBuffer(socket_master,sizeof(int),&(datos.fileSize));
	datos.file=malloc(datos.fileSize);
	readBuffer(socket_master,datos.fileSize,datos.file);

//OBTENGO CONTENIDO DE LOS BLOQUES
	readBuffer(socket_master,sizeof(int),&(datos.blocksSize));
	datos.blocks=malloc(sizeof(block)*datos.blocksSize);
	for(i=0; i<datos.blocksSize; ++i){
		readBuffer(socket_master,sizeof(int),&(datos.blocks[i].pos));
		readBuffer(socket_master,sizeof(int),&(datos.blocks[i].size));
		readBuffer(socket_master,sizeof(char)*28,&(datos.blocks[i].tmp));
		printf("\tpos:%d\tsize:%d\ttmp:%s\n", datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp);
	}

	log_info(logger,"Master %d: Datos de transformación obtenidos",socket_master);
	log_info(logger,"Master %d: Almacenando Script de transformación",socket_master);
	char* scriptName = regenerateScript(datos.file,script_transform,'T',socket_master);

//PROCESO Y GENERO RESPUESTA
	tr_node_rs* answer = malloc(sizeof(tr_node_rs));
	int buffSize = sizeof(int)+sizeof(char);
	void* buff = malloc(buffSize);
	log_info(logger,"Master %d: Iniciando transformación",socket_master);
	for(i = 0; i < (datos.blocksSize); ++i){ //REEMPLAZAR POR FORKS
		answer->block = datos.blocks[i].pos;
		answer->result=transformBlock(datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp, scriptName);
	//SERIALIZO RESPUESTA
		memcpy(buff,&(answer->block),sizeof(int));
		memcpy(buff+sizeof(int),&(answer->result),sizeof(char));
		if(send(socket_master,buff,buffSize,0)<0){
			log_warning(logger,"Master %d: Desconectado, no es posible comunicar el resultado", socket_master);
		};
	}
	free(buff);
	free(answer);

	//------------

	free(scriptName);
	free(datos.blocks);
	free(datos.file);
	log_trace(logger, "Master %d: Transformación finalizada", socket_master);
}

//================================================================================

char transformBlock(int position, int size, char temporal[28], char* scriptName){
	log_info(logger,"Bloque %d: Transformando...", position);

//OBTENGO CONTENIDO DEL BLOQUE DEL .BIN
	char* blockContent = NULL;
	//blockContent = malloc(size);

//LECTURA DEL BLOQUE CON VALIDACIÓN
	blockContent = getBlockData(position, size);
		if(!blockContent){
			log_error(logger,"Bloque %d: No se pudo leer el contenido del bloque");
			return 'E';
		}
//GUARDO EL CONTENIDO EN UN TEMPORAL
	char* contentFileName;
	contentFileName=generateBlockFile(blockContent,position);

//GENERO COMANDO PARA TRANSFORMACIÓN
	char* command;
	command = malloc(size+26+12+28+1+3+3+2);
	asprintf(&command, "cat %s | ./%s | sort > %s",contentFileName+1, scriptName, temporal+1);
	printf("Comando: %s\n",command);
	free(blockContent);
	free(contentFileName);

//EJECUTO COMANDO Y ATAJO EL ERROR
	if (system(command)!=0){
		log_error(logger,"Bloque %d: Falló la transformación", position);
		free(command);
		return 'E';
	}else{
		log_info(logger,"Bloque %d: Transformación finalizada", position);
		free(command);
		return 'O';
	}
}
