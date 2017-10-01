/*
 * yamaPlanner.c
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#include "yamaPlanner.h"
#include <string.h>

void doPlanning(Yama* yama, t_list* nodeList) {
	//if (strcmp(yama->balancign_algoritm, "WRR") == 0)
		//hRoundRobinPlanning(yama, nodeList);

	//if (strcmp(yama->balancign_algoritm, "RR") == 0)
		//roundRobinPlanning(yama, nodeList);
}

//WLmax: la mayor carga de trabajo existente entre todos Workers
//WL(w): la carga de trabajo actual del Worker.

void hclockPlanning(Yama* yama, t_list* nodeList) {

}

void clockPlanning(Yama* yama, t_list* nodeList) {

}

/*tr_datos* chooseNode(Yama* yama, tr_datos* nodeCopy1, tr_datos* nodeCopy2){

}*/
