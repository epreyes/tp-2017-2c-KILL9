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
#include "headers/utils.h"
#include "headers/answers.h"

#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStorage.h"
#include "headers/connect.h"
#include "headers/connectionsManager.h"

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){

//--preparo metricas---------

	struct timeval start,end;
	gettimeofday(&start,NULL);

//--Logger


//--recibo respuestas de Yama

	//int fileNameSize = strlen(argv[3]);
	int answerSize_TR = sizeof(tr_answer)/sizeof(tr_answer[0]);
	int answerSize_RL = sizeof(rl_answer)/sizeof(rl_answer[0]);
	int answerSize_RG = sizeof(rg_answer)/sizeof(rg_answer[0]);
	int answerSize_AF = sizeof(af_answer)/sizeof(af_answer[0]);

	//workerConnect();
	loadConfigs();
	openYamaConnection();

	createLoggers();
	validateArgs(argc, argv);									//valido argumentos

	transformFile(tr_answer,answerSize_TR,&masterMetrics,argv[3]);		//ordena ejecución de transformacion

/*
	runLocalReduction(rl_answer,answerSize_RL,&masterMetrics);	//ordena ejecución de Reductor Local
	runGlobalReduction(rg_answer,answerSize_RG,&masterMetrics);	//ordena ejecución de Reductor Global
	saveResult(af_answer,answerSize_AF,&masterMetrics);			//ordena guardado en FileSystem

	gettimeofday(&end,NULL);
	masterMetrics.runTime = timediff(&end,&start);
	printMetrics(masterMetrics);
*/
	log_destroy(logger);
	config_destroy(config);

	return EXIT_SUCCESS;
};
