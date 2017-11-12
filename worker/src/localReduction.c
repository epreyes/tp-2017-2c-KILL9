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

//OBTENIENDO SCRIPT REDUCTOR
	readBuffer(socket_master,sizeof(int),&(datos.fileSize));
	datos.file=malloc(datos.fileSize);
	readBuffer(socket_master,datos.fileSize,datos.file);
	readBuffer(socket_master,28,&(datos.rl_tmp));			//nombre de archivo de salida

//OBTENIENDO DATOS DE ARCHIVOS A REDUCIR
	readBuffer(socket_master,sizeof(int),&(datos.tmpsQuantity));
	datos.tr_tmp = malloc(28*(datos.tmpsQuantity));
	for(i=0; i<datos.tmpsQuantity; ++i){
		readBuffer(socket_master,sizeof(tmp_tr),&(datos.tr_tmp[i]));
		//printf("FILE:%s\n", datos.tr_tmp[i]);
	}
	//printf("\nCANTIDAD:%d\n", datos.tmpsQuantity);
	log_info(logger,"Master %d: Datos de Reducción Local Obtenidos",socket_master);
	log_info(logger,"Master %d: Guardando script de reducción", socket_master);
	char* scriptName = regenerateScript(datos.file,script_reduction,'R',socket_master);
//GENERO RESPUESTA
	int bufferSize = sizeof(char);
	rl_node_rs* answer = malloc(sizeof(rl_node_rs));
	void* buffer = malloc(bufferSize);
//PROCESO
	answer->result= reduceFiles(datos.tmpsQuantity, datos.tr_tmp, scriptName, datos.rl_tmp);
//SERIALIZO
	memcpy(buffer,&(answer->result),sizeof(char));
	if(send(socket_master,buffer,bufferSize,0)<0){
		log_warning("Master %d: Desconectado, no es posible comunicar el resultado", socket_master);
	};

	free(answer);
	free(buffer);

	free(datos.tr_tmp);
	free(datos.file);
	log_trace(logger, "Master %d: Reducción local finalizada", socket_master);
};


char reduceFiles(int filesQuantity, tmp_tr* filesNames, char* script, char* reducedFileName){
	int i=0;
	char file1[28], file2[28], mergedFile[28];//Validar tamaños

	log_info(logger,"Iniciando apareo de archivos");
//OBTENGO EL PRIMERO
	strcpy(file1,filesNames[i]);
	strcpy(mergedFile,file1+1);//VALIDO POR SI SOLO HAY UNO

//MERGEO TODOS LOS TEMPORALES
	for (i = 1; i < filesQuantity; ++i){
		strcpy(file2,filesNames[i]);
		strcpy(mergedFile, generateScriptName('X',i));
		//printf("FILE1:%s\nFILE2:%s\n",file1,file2);
		mergeFiles(file1,file2,mergedFile);
		//printf("MERGEDFILE:%s\n",mergedFile);
		strcpy(file1,mergedFile);
	}
	log_info(logger,"Apareo de archivos finalizado");
	log_info(logger,"Preparando para ejecución de reducción");
//GENERO COMANDO PARA EJECUTAR REDUCCION
	char* command;
	command = malloc(strlen(mergedFile)+strlen(script)+strlen(reducedFileName)+13);
	asprintf(&command,"cat %s | %s > %s",mergedFile,script,reducedFileName+1);
	printf("\nCOMANDO\n:%s", command);
//EJECUTO REDUCCION
	if (system(command)!=0){
		log_error(logger,"Falló la reducción local");
		free(command);
		return 'E';
	}else{
		log_trace(logger,"Reducción local finalizada");
		free(command);
		return 'O';
	}
/*
*/
}
