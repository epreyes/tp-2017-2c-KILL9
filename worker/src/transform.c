/*
 * transform.c
 *
 *  Created on: 17/10/2017
 *      Author: utnso
 */
#include "headers/transform.h"

void transformation(int socketClient){
	tr_node datos;
	int i=0;
	log_info(logger,"Master %d: Obteniendo datos de transformación",socketClient);

	//Obteniendo Script INFO
	readBuffer(socketClient,sizeof(int),&(datos.fileSize));
	datos.file=malloc(datos.fileSize);
	readBuffer(socketClient,datos.fileSize,datos.file);

	//Obteniendo Blocks INFO
	readBuffer(socket_master,sizeof(int),&(datos.blocksSize));
	datos.blocks=malloc(sizeof(block)*datos.blocksSize);
	for(i=0; i<datos.blocksSize; ++i){
		readBuffer(socketClient,sizeof(int),&(datos.blocks[i].pos));
		readBuffer(socketClient,sizeof(int),&(datos.blocks[i].size));
		readBuffer(socketClient,sizeof(char)*28,&(datos.blocks[i].tmp));
		printf("\tpos:%d\tsize:%d\ttmp:%s\n", datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp);
	}

	log_info(logger,"Master %d: Datos de transformación obtenidos",socketClient);

	char* scriptName = regenerateScript(datos.file,script_transform,'T',socketClient);

	//------------
	tr_node_rs* answer = malloc(sizeof(tr_node_rs));
	void* buff = malloc(sizeof(int)+sizeof(char)+sizeof(int));

	pid_t pid ;
	for (i = 0; i < (datos.blocksSize); ++i){ //REEMPLAZAR POR FORKS
		pid = fork();

		if (pid == 0 ){
	   		log_trace(logger, "Proceso Hijo PID %d (PID del padre: %d)",getpid(),getppid());
	   		answer->block = datos.blocks[i].pos;
			answer->result=transformBlock(datos.blocks[i].pos,datos.blocks[i].size,datos.blocks[i].tmp, scriptName);
			answer->runtime=12;
			//serializo
			memcpy(buff,&(answer->block),sizeof(int));
			memcpy(buff+sizeof(int),&(answer->result),sizeof(char));
			memcpy(buff+sizeof(int)+sizeof(char),&(answer->runtime),sizeof(int));
			send(socketClient,buff,sizeof(int)+sizeof(char)+sizeof(int),0);
			log_trace(logger, "Fin de Proceso Hijo PID %d",getpid());
		}
	}
	free(buff);
	free(answer);

	free(datos.blocks);
	free(datos.file);
	log_trace(logger, "Master %d: Transformación finalizada", socketClient);
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
