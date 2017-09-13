/*
 * finalStorage.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/finalStorage.h"


int saveResult(af_datos yamaAnswer[], int totalRecords, metrics *masterMetrics){

	struct timeval af_start,af_end;
	gettimeofday(&af_start,NULL);

	//conectarseConWorker y pasar yamaAnswer.rg_tmp

	gettimeofday(&af_end,NULL);
	masterMetrics->finalStorage.runTime = timediff(&af_end,&af_start);
	masterMetrics->finalStorage.errors = 0;
	return EXIT_SUCCESS;
}
