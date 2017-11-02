/*
 ============================================================================
 Name        : master.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Master Process
 ============================================================================
 */

//=========LIBREARIES==========//

#include "headers/master.h"
#include "headers/metrics.h"
#include "headers/answers.h"

#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStorage.h"

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){
//--preparo metricas---------
	struct timeval start,end;
	gettimeofday(&start,NULL);

	createLoggers();
	loadConfigs();
	validateArgs(argc, argv);										//valido argumentos
	openYamaConnection();
	loadScripts(argv[1],argv[2]);

	transformFile(&masterMetrics,argv[3]);	//ordena ejecución de transformacion
	runLocalReduction(&masterMetrics);		//ordena ejecución de Reductor Local

	runGlobalReduction(&masterMetrics);		//ordena ejecución de Reductor Global
	/*
	saveResult(&masterMetrics, argv[4]);				//ordena guardado en FileSystem

	gettimeofday(&end,NULL);
	masterMetrics.runTime = timediff(&end,&start);
	printMetrics(masterMetrics);
	*/

//--cierro todo lo grobal
	fclose(script_transform);
	fclose(script_reduction);

	log_trace(logger,"JOB FINALIZADO");
	log_destroy(logger);
	config_destroy(config);

	return EXIT_SUCCESS;
};
