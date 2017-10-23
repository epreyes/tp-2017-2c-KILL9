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

	transformFile(/*tr_answer,answerSize_TR,*/&masterMetrics,argv[3]);	//ordena ejecución de transformacion
	/*
	runLocalReduction(rl_answer,answerSize_RL,&masterMetrics);		//ordena ejecución de Reductor Local
	runGlobalReduction(rg_answer,answerSize_RG,&masterMetrics);		//ordena ejecución de Reductor Global
	saveResult(af_answer,answerSize_AF,&masterMetrics);				//ordena guardado en FileSystem

	gettimeofday(&end,NULL);
	masterMetrics.runTime = timediff(&end,&start);
	printMetrics(masterMetrics);
	*/

//--cierro todo lo grobal
	fclose(script_transform);
	fclose(script_reduction);
	log_destroy(logger);
	config_destroy(config);

	return EXIT_SUCCESS;
};
