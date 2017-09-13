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


///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){

//--preparo metricas---------

	struct timeval start,end;
	gettimeofday(&start,NULL);

//--Logger


//--recibo respuestas de Yama

	int answerSize_TR = sizeof(tr_answer)/sizeof(tr_answer[0]);
	int answerSize_RL = sizeof(rl_answer)/sizeof(rl_answer[0]);
	int answerSize_RG = sizeof(rg_answer)/sizeof(rg_answer[0]);
	int answerSize_AF = sizeof(af_answer)/sizeof(af_answer[0]);

	createLoggers();
	validateConfigs();
	validateArgs(argc, argv);									//valido argumentos
	transformFile(tr_answer,answerSize_TR,&masterMetrics);		//ordena ejecución de transformacion
	//log_info(logger,"Transformación finalizada");
	runLocalReduction(rl_answer,answerSize_RL,&masterMetrics);	//ordena ejecución de Reductor Local
	runGlobalReduction(rg_answer,answerSize_RG,&masterMetrics);	//ordena ejecución de Reductor Global
	saveResult(af_answer,answerSize_AF,&masterMetrics);			//ordena guardado en FileSystem

	gettimeofday(&end,NULL);
	masterMetrics.runTime = timediff(&end,&start);
	printMetrics(masterMetrics);

	log_destroy(logger);

	return EXIT_SUCCESS;
};
