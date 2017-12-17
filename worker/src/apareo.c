/*
 * apareo.c
 *
 *  Created on: 27/10/2017
 *      Author: utnso
 */

#include "headers/apareo.h"

#define LONG 1024

char *minorWord(char* word1, char* word2){
	return (strcmp(word1, word2)<0)?word1:word2;
};
/*
	int pos=0;
	for(pos=0; (word1[pos]!='\0' && word2[pos]!='\0'); ++pos){
		if(word1[pos]<word2[pos])return word1;
		if(word1[pos]>word2[pos])return word2;
	}
	return (word1[pos]=='\0')?word1:word2;
}
*/

void readWord(char *word, FILE *file, int* endFile){
	if(!feof(file)){
		fgets(word,LONG,file);
	}else{
		endFile[0] = 1;
	}
}


int mergeFiles(char* Aname, char* Bname, char *Cname){
	char* word1=malloc(LONG);
	char* word2=malloc(LONG);
	int endFile1=0, endFile2=0;

	FILE *A,*B,*C;

	if((A=fopen(Aname,"r"))==NULL)log_warning(logger,"No se encuentra el archivo %s", Aname);
	if((B=fopen(Bname,"r"))==NULL)log_warning(logger,"No se encuentra el archivo %s", Bname);
	if((C=fopen(Cname,"w"))==NULL)log_warning(logger,"No se pudo crear el archivo %s", Cname);

	if(!A || !B || !C){
		fclose(A);
		fclose(B);
		fclose(C);
		log_error(logger,"No es posible continuar con el Apareo");
		return EXIT_FAILURE;
	}

	log_info(logger,"Archivos preparados para el apareo");

	readWord(word1, A, &endFile1);
	readWord(word2, B, &endFile2);

	while(!endFile1 || !endFile2){
		if(endFile2 || (!endFile1 && (minorWord(word1,word2)==word1))){
			fputs(word1,C);
			readWord(word1, A, &endFile1);
		}else{
			fputs(word2,C);
			readWord(word2, B, &endFile2);
		}
	}
	fclose(A);
	fclose(B);
	fclose(C);
	log_info(logger,"Apareo finalizado entre %s y %s", Aname, Bname);

	free(word1);
	free(word2);
	return EXIT_SUCCESS;
};






int mergeBySystem(int filesCuantity, tmp_tr* files, char* mergedFile){

	//printf("files:%d",filesCuantity);
	typedef char space[29];
	int i;
	char* command;

	char filesList[filesCuantity*(29)]; //= malloc(filesCuantity*(sizeof(tmp_tr)+1));
	filesList[0]='\0';
	space filesB[filesCuantity];

	for(i=0;i<filesCuantity;i++){
		strcpy(filesB[i], files[i]);
		strcat(filesB[i]," ");
		strcat(filesList, (filesB[i]+1));
	}
	command = malloc(29*filesCuantity+100);

	log_trace(logger,"Apareo de archivos iniciado...");
	asprintf(&command,"sort -m %s > %s", filesList, mergedFile+1);
	//printf("\ncommand:%s", command);
//EJECUTO REDUCCION
	if (system(command)!=0){
		//usleep(1000000);
		log_error(logger,"No se pudo finalizar el apareo de archivos");
		free(command);
		return EXIT_FAILURE;
	}else{
		log_trace(logger,"Apareo de archivos finalizado");
		free(command);
		return EXIT_SUCCESS;
	}
};
