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

#include "headers/transform.h"
#include "headers/localReduction.h"
#include "headers/globalReduction.h"
#include "headers/finalStorage.h"

///////////MAIN PROGRAM///////////

int main(int argc, char* argv[]){

//PREPARO
	abortJob = '0';
	struct timeval start,end;
	gettimeofday(&start,NULL);
	createLoggers();
	validateArgs(argc, argv);
	loadConfigs();
	loadScripts(argv[1],argv[2]);
	openYamaConnection();


//PROCESO
	transformFile(argv[3]);				//RUN TRANSFORMATION
	if(abortJob!='T'){
		runLocalReduction();			//RUN LOCAL REDUCTION
		if(abortJob!='L'){
			runGlobalReduction();		//RUN GLOBAL REDUCTION
			if(abortJob!='G')
				saveResult(argv[4]);	//RUN FINAL STORAGE
		}
	}

//IMPRIMO MÉTRICAS

	gettimeofday(&end,NULL);
	masterMetrics.runTime = timediff(&end,&start);
	printMetrics(masterMetrics);

//FINALIZO

	fclose(script_transform);
	fclose(script_reduction);

	(abortJob=='0')?log_trace(logger,"JOB FINALIZADO"):log_error(logger,"JOB ABORTADO");
	log_destroy(logger);
	config_destroy(config);

	//cerrar sockets

	return EXIT_SUCCESS;
};
