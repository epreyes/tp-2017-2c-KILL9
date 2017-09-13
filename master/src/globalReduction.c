/*
 * globalReduction.c
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#include "headers/globalReduction.h"


////////////////////////////////////GLOBAL_REDUCTION/////////////////////////////////////////

int runGlobalReduction(rg_datos yamaAnswer[], int totalRecords, metrics *masterMetrics){
	struct timeval gr_start,gr_end;
	gettimeofday(&gr_start,NULL);

	int recordCounter=0;
	dataNodes *brothersData = NULL;
	brothersData = (dataNodes*) malloc(sizeof(dataNodes));
	dataThread_GR* dataThread = NULL;
	dataThread = (dataThread_GR*) malloc(sizeof(dataThread_GR));

	//recorro los registros de la respuesta
	while(recordCounter<totalRecords){
		if(yamaAnswer[recordCounter].encargado==1){
			dataThread->leadNode = yamaAnswer[recordCounter].nodo;
			strcpy(dataThread->leadConector, yamaAnswer[recordCounter].direccion);
			strcpy(dataThread->rg_tmp, yamaAnswer[recordCounter].rg_tmp);
		}else{
			brothersData = (dataNodes*)realloc(brothersData,(sizeof(dataNodes)*(recordCounter+1)));
			strcpy(brothersData[recordCounter].conector, yamaAnswer[recordCounter].direccion);
			brothersData[recordCounter].node = yamaAnswer[recordCounter].nodo;
			strcpy(brothersData[recordCounter].rl_tmp, yamaAnswer[recordCounter].rl_tmp);
		}
		recordCounter++;
	};

	dataThread->brothersData = (dataNodes *) malloc(sizeof(dataNodes)*recordCounter);
	dataThread->brothersData = (dataNodes *) memcpy(dataThread->brothersData,brothersData, sizeof(dataNodes)*recordCounter);
	dataThread->brothersCount = recordCounter;


	free(brothersData);
	free(dataThread->brothersData);
	free(dataThread);

	gettimeofday(&gr_end,NULL);
	masterMetrics->globalReduction.runTime = timediff(&gr_end,&gr_start);

	return EXIT_SUCCESS;
}
