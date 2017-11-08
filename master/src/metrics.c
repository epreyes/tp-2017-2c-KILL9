/*
 * metrics.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/metrics.h"
#include <stdio.h>
#include <stdlib.h>

void printSeparator(char c){
	int i;
	for (i=0; i <= 52; ++i) printf("%c",c);
	printf("\n");
}

void printProcessMetrics(char* name, procMetrics metrics){
	printf("%s\n", name);
	printSeparator('-');
	if(strcmp(name,"ALMACENADO FINAL")){
		printf("Cantidad total de tareas:\t\t%d\n", metrics.tasks);
		if(strcmp(name,"REDUCCIÓN GLOBAL"))
			printf("Cantidad de tareas paralelas:\t\t%d\n", metrics.parallelTask);
	}
	printf("Tiempo de ejecución promedio:\t\t%.6gms\n", metrics.runTime);
	printf("Cantidad de errores:\t\t\t%d\n", metrics.errors);
	printSeparator('=');
}

void printErrorProcess(char* name){
	printf("%s\n", name);
	printf("No se pudo procesar esta operación\n");
	printSeparator('-');
}


void printMetrics(metrics masterMetrics){
	log_trace(logger,"Preparando impresión de métricas");

//MÉTRICAS GENERALES
	printSeparator('=');
	printf("TIEMPO DE EJECUCIÓN TOTAL:\t\t%.6gms\n", masterMetrics.runTime);
	printSeparator('-');
	printf("CANTIDAD DE FALLOS TOTAL:\t\t%d\n", masterMetrics.transformation.errors+masterMetrics.localReduction.errors+masterMetrics.globalReduction.errors+masterMetrics.finalStorage.errors);
	printSeparator('=');

//MÉTRICAS POR PROCESO

	switch(abortJob){
		case '0':
			printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
			printProcessMetrics("REDUCCIÓN LOCAL", masterMetrics.localReduction);
			printProcessMetrics("REDUCCIÓN GLOBAL", masterMetrics.globalReduction);
			printProcessMetrics("ALMACENADO FINAL",masterMetrics.finalStorage);
			break;
		case 'T':
			printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
			printErrorProcess("TRANSFORMACIÓN");
			printErrorProcess("REDUCCIÓN LOCAL");
			printErrorProcess("REDUCCIÓN GLOBAL");
			printErrorProcess("ALMACENADO FINAL");
			break;
		case 'L':
			printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
			printErrorProcess("REDUCCIÓN LOCAL");
			printErrorProcess("REDUCCIÓN GLOBAL");
			printErrorProcess("ALMACENADO FINAL");
			break;
		case 'G':
			printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
			printProcessMetrics("REDUCCIÓN LOCAL", masterMetrics.localReduction);
			printErrorProcess("REDUCCIÓN GLOBAL");
			printErrorProcess("ALMACENADO FINAL");
			break;
		case 'S':
			printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
			printProcessMetrics("REDUCCIÓN LOCAL", masterMetrics.localReduction);
			printProcessMetrics("REDUCCIÓN GLOBAL", masterMetrics.globalReduction);
			printErrorProcess("ALMACENADO FINAL");
			break;
	}
	log_trace(logger,"Impresión de métricas finalizada");
}
