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

	//Obteniendo Script INFO
	readBuffer(socket_master,sizeof(int),&(datos.fileSize));
	datos.file=malloc(datos.fileSize);
	readBuffer(socket_master,datos.fileSize,datos.file);

	//Obteniendo Blocks INFO
	readBuffer(socket_master,sizeof(int),&(datos.blocksSize));
	datos.blocks=malloc(sizeof(block)*datos.blocksSize);
	for(i=0; i<datos.blocksSize; ++i){
		readBuffer(socket_master,sizeof(int),&(datos.blocks[i].pos));
		readBuffer(socket_master,sizeof(int),&(datos.blocks[i].size));
		readBuffer(socket_master,sizeof(char)*28,&(datos.blocks[i].tmp));
		printf("\tpos:%d\tsize:%d\ttmp:%s\n", datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp);
	}

	log_info(logger,"Master %d: Datos de transformación obtenidos",socket_master);

	char* scriptName = regenerateScript(datos.file,script_transform,'T',socket_master);

	//------------
	tr_node_rs* answer = malloc(sizeof(tr_node_rs));
	void* buff = malloc(sizeof(int)+sizeof(char)+sizeof(int));

	for (i = 0; i < (datos.blocksSize); ++i){ //REEMPLAZAR POR FORKS
		answer->block = datos.blocks[i].pos;
		answer->result=transformBlock(datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp, scriptName);
		answer->runtime=12;
		//serializo
		memcpy(buff,&(answer->block),sizeof(int));
		memcpy(buff+sizeof(int),&(answer->result),sizeof(char));
		memcpy(buff+sizeof(int)+sizeof(char),&(answer->runtime),sizeof(int));
		send(socket_master,buff,sizeof(int)+sizeof(char)+sizeof(int),0);
	}
	free(buff);
	free(answer);

	//------------

	free(datos.blocks);
	free(datos.file);
	log_trace(logger, "Master %d: Transformación finalizada", socket_master);
}

//================================================================================

char transformBlock(int position, int size, char temporal[28], char* scriptName){
	log_info(logger,"Bloque %d: Transformando...", position);

	//-----Obtengo contenido del bloque-----------------------------
	char* blockContent = NULL;
	blockContent = malloc(size);
	asprintf(&blockContent, "\"Contenido del bloque %d\"",position);
	blockContent = getBlockData(position, size);

	//--------------------------------------------------------------
	//sizeBlock+temp[26]+command[12]+temp[28]+\0[1]
	char* command = NULL;
	command = malloc(size+26+12+28+1+3+3);
	asprintf(&command, "cat %s | %s | sort > %s",blockContent, scriptName, temporal+1);

	free(blockContent);

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
