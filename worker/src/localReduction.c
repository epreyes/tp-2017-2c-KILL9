/*
 * localReduction.c
 *
 *  Created on: 18/10/2017
 *      Author: utnso
 */

#include "headers/localReduction.h"

void localReduction(){
	rl_node datos;
	int i=0;
	log_info(logger,"Master %d: Obteniendo datos de reducción local",socket_master);

//OBTENIENDO SCRIPT REDUCCTOR
	readBuffer(socket_master,sizeof(int),&(datos.fileSize));
	datos.file=malloc(datos.fileSize);
	readBuffer(socket_master,datos.fileSize,datos.file);
	readBuffer(socket_master,sizeof(tmp),&(datos.rl_tmp));			//nombre de archivo de salida

	char* scriptName = regenerateScript(datos.file,script_reduction,'R',socket_master);

//OBTENIENDO DATOS DE ARCHIVOS A REDUCIR
	readBuffer(socket_master,sizeof(int),&(datos.tmpsQuantity));
	datos.tr_tmp=malloc(sizeof(tmp)*datos.tmpsQuantity);
	for(i=0; i<datos.tmpsQuantity; ++i){
		readBuffer(socket_master,sizeof(tmp),&(datos.tr_tmp[i]));
	}

	log_info(logger,"Master %d: Datos de Reducción Local Obtenidos",socket_master);

//GENERO RESPUESTA
	int bufferSize = sizeof(int)+sizeof(char);
	rl_node_rs* answer = malloc(sizeof(tr_node_rs));
	void* buffer = malloc(bufferSize);
//PROCESO
	answer->result= reduceFiles(datos.tmpsQuantity, datos.tr_tmp, scriptName);
	answer->runTime=12; //CAMBIAR

	//serializo
	memcpy(buffer,&(answer->result),sizeof(char));
	memcpy(buffer+sizeof(char),&(answer->runTime),sizeof(int));
	send(socket_master,buffer,bufferSize,0);

	free(buffer);
	free(answer);

	//------------

	free(datos.tr_tmp);
	free(datos.file);
	log_trace(logger, "Master %d: Transformación finalizada", socket_master);
};


char reduceFiles(int filesQuantity, tmp* filesNames, char* script, char* reducedFileName){

	int i=0;
	char file1[sizeof(tmp)], file2[sizeof(tmp)], mergedFile[sizeof(tmp)]; //Validar tamaños

	//OBTENGO EL PRIMERO
	strcpy(file1,filesNames[i]);
	strcpy(mergedFile,file1);//VALIDO POR SI SOLO HAY UNO

	//MERGEO TODOS LOS TEMPORALES
	for (i = 1; i < filesQuantity; ++i){
		strcpy(file2,filesNames[i]);
		strcpy(mergedFile, generateScriptName('X',i));
		mergeFiles(file1,file2,mergedFile);
		strcpy(file1,mergedFile);
	}

	//GENERO COMANDO PARA EJECUTAR REDUCCION
	char* command;
	command = malloc(strlen(mergedFile)+strlen(script)+strlen(reducedFileName)+9);
	asprintf(&command,"%s | %s > %s",mergedFile,script,reducedFileName);

	//EJECUTO REDUCCION
	system(command);

	//FINALIZO
	free(command);
	return 'O'; // 'E'
}
