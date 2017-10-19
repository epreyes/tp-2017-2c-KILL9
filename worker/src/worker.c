/*
 ============================================================================
 Name        : Worker.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "headers/worker.h"
#include "headers/connectionsManager.h"
#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStore.h"
#include "headers/utils.h"


int main(){
	//loadServer();
	//resultados Hardcodeados//
	int position = 65, size = 100000;
	char temporal[28] = "tmp/1710131859-T-M000-B068";
	char operation = 'T';
	//-------------
	generateTempsFolder();
	regenerateScript("hola mundo",transformScript,"archivo");
	switch(operation){
		case 'T':
			//recorrer y generar un fork por cada bloque
			//cada transformBlock va a hacer el send a Master
				transformBlock(position,size,temporal);
			//}
			break;
		case 'L':
			//reduceFile(file[],reductionScript);
			break;
		case 'G':
				//globalReduction(files[],reductionScript);
			break;
		case 'S':
			//finalStorage(???,???);
			break;
	}
	return EXIT_SUCCESS;
}
