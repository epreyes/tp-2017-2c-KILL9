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
		printf("\tFILE:%s\n", datos.tr_tmp[i]);
	}
	printf("\nCANTIDAD:%d\n", datos.tmpsQuantity);
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
	if(send(socket_master,buffer,bufferSize,0)<=0){
		log_warning(logger,"Master %d: Desconectado, no es posible comunicar el resultado", socket_master);
	};

	free(answer);
	free(buffer);
//	free(datos.tr_tmp);
//	free(datos.file);
	log_trace(logger, "Master %d: Reducción local finalizada", socket_master);
};


char reduceFiles(int filesQuantity, tmp_tr* filesNames, char* script, char* reducedFileName){
	//int i=0;
	char file1[28], file2[28];
	char mergedFile[28];
	log_info(logger,"Iniciando etapa de reducción");

//OBTENGO EL PRIMERO
	//strcpy(file1,filesNames[i]);
	//strcpy(mergedFile,file1);//VALIDO POR SI SOLO HAY UNO

	if(filesQuantity>1){
		strcpy(mergedFile, generateAuxFile());
		mergeBySystem(filesQuantity, filesNames, mergedFile);
	}else{
		strcpy(mergedFile,filesNames[0]);
	}
		/*
//MERGEO TODOS LOS TEMPORALES
	for (i = 1; i < filesQuantity; ++i){
	log_info(logger,"Iniciando apareo de archivos");
		strcpy(file2,filesNames[i]);
		strcpy(mergedFile, generateAuxFile());
			printf("\nFILE1:%s\nFILE2:%s\n",file1,file2);
		if(mergeFiles(file1,file2,mergedFile)==1){
			log_error(logger,"Falló el mergeo de archivos");
			return 'E';
		};
			printf("\nMERGEDFILE:%s\n",mergedFile);
		strcpy(file1,mergedFile);
	log_info(logger,"Apareo de archivos finalizado");
	}
*/
	log_info(logger,"Preparando para ejecución de reducción");

//GENERO COMANDO PARA EJECUTAR REDUCCION
	char* command = malloc(strlen(mergedFile)+strlen(script)+strlen(reducedFileName)+15);
	asprintf(&command,"cat %s | ./%s > %s",mergedFile+1,script,reducedFileName+1);
	//printf("\nCOMANDO:%s", command);

//EJECUTO REDUCCION
	if (system(command)!=0){
		log_error(logger,"Falló la reducción");
		//usleep(1000000);
		free(command);
		return 'E';
	}else{
		log_trace(logger,"Reducción finalizada");
		//usleep(1000000);
		free(command);
		return 'O';
	}
}
