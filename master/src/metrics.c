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
	printf("Cantidad total de tareas:\t\t%d\n", 40);
	printf("Cantidad de tareas paralelas:\t\t%d\n", 40);
	printf("Cantidad de errores:\t\t\t%d\n", 0);
	printSeparator('=');
}

void printMetrics(metrics masterMetrics){
	//Tiempos de ejecucion
		printSeparator('=');
		printf("TIEMPO DE EJECUCIÓN TOTAL:\t\t%.6gms\n", masterMetrics.runTime);
		printSeparator('-');
		printf("> Transformacion\t\t\t%.6gms\n", masterMetrics.transformation.runTime);
		printf("> LocalReduction\t\t\t%.6gms\n", masterMetrics.localReduction.runTime);
		printf("> GlobalReduction\t\t\t%.6gms\n", masterMetrics.globalReduction.runTime);
		printf("> FinalStorage\t\t\t\t%.6gms\n", masterMetrics.finalStorage.runTime);
		printSeparator('=');

	//Imprimo las métricas de cada proceso
		printProcessMetrics("TRANSFORMACIÓN", masterMetrics.transformation);
		printProcessMetrics("REDUCCIÓN LOCAL", masterMetrics.localReduction);
		printProcessMetrics("REDUCCIÓN GLOBAL", masterMetrics.globalReduction);
		printProcessMetrics("ALMACENADO FINAL",masterMetrics.finalStorage);
}
