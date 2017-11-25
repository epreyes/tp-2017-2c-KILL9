/*
 * apareo.c
 *
 *  Created on: 27/10/2017
 *      Author: utnso
 */

#include "headers/apareo.h"


char *minorWord(char* word1, char* word2){
	int pos=0;
	for(pos=0; (word1[pos]!='\0' && word2[pos]!='\0'); ++pos){
		if(word1[pos]<word2[pos])return word1;
		if(word1[pos]>word2[pos])return word2;
	}
	return (word1[pos]=='\0')?word1:word2;
}

void readWord(char *word, FILE *file, int* endFile){
	if(!feof(file)){
		int i=0;
		fgets(word,128,file);
		for(i=0;word[i]!='\0';++i);
			if(word[i-1]!='\n'){
				word[i]='\n';
				word[i+1]='\0';
			}
	}else{
		endFile[0] = 1;
	}
}


int mergeFiles(char* Aname, char* Bname, char *Cname){
	char word1[256], word2[256];
	int endFile1=0, endFile2=0;

	FILE *A,*B,*C;

	if((A=fopen(Aname+1,"r"))==NULL)log_warning(logger,"No se encuentra el archivo %s", Aname);
	if((B=fopen(Bname+1,"r"))==NULL)log_warning(logger,"No se encuentra el archivo %s", Bname);
	if((C=fopen(Cname+1,"w"))==NULL)log_warning(logger,"No se pudo crear el archivo %s", Cname);

	if(!A || !B || !C){
		fclose(A);
		fclose(B);
		fclose(C);
		log_error(logger,"No es posible continuar con el Apareo");
		return EXIT_FAILURE;
	}

	log_info(logger,"Archivos preparados para el apareo");

	readWord(&word1[0], A, &endFile1);
	readWord(&word2[0], B, &endFile2);

	while(!endFile1 || !endFile2){
		if(endFile2 || (!endFile1 && (minorWord(word1,word2)==word1))){
			fputs(word1,C);
			readWord(&word1[0], A, &endFile1);
		}else{
			fputs(word2,C);
			readWord(&word2[0], B, &endFile2);
		}
	}
	fclose(A);
	fclose(B);
	fclose(C);
	log_info(logger,"Apareo finalizado entre %s y %s", Aname, Bname);
	return EXIT_SUCCESS;
};

int mergeBySystem(int filesCuantity, tmp_tr* files, char* mergedFile){

	typedef char space[29];

	char* command = NULL;
	char filesList[filesCuantity*(sizeof(tmp_tr)+1)]; //= malloc(filesCuantity*(sizeof(tmp_tr)+1));
	filesList[0]='\0';
	space filesB[filesCuantity];
	int i;

	for(i=0;i<filesCuantity;i++){
		strcpy(filesB[i], strcat(files[i]+1," "));
		strcpy(filesList, strcat(filesList, filesB[i]));
	}

	command = malloc(29*filesCuantity+13);
	asprintf(&command,"cat %s| sort > %s", filesList, mergedFile+1);
	//log_info(logger, "%s",command);

//EJECUTO REDUCCION
	if (system(command)!=0){
		log_error(logger,"No se pudo finalizar el apareo de archivos");
		free(command);
		return EXIT_FAILURE;
	}else{
		log_trace(logger,"Apareo de archivos finalizado");
		free(command);
		return EXIT_SUCCESS;
	}
};
