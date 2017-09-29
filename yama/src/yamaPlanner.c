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
		hRoundRobinPlanning(yama, nodeList);

	//if (strcmp(yama->balancign_algoritm, "RR") == 0)
		roundRobinPlanning(yama, nodeList);
}

void hRoundRobinPlanning(Yama* yama, t_list* nodeList) {

}

void roundRobinPlanning(Yama* yama, t_list* nodeList) {
	int index = 0;
	for (index = 0; index < list_size(nodeList); index+=2) {
		tr_datos* nodeCopy1 = list_get(nodeList, index);
		tr_datos* nodeCopy2 = list_get(nodeList, index+1);
		//tr_datos* choosed = chooseNode(yama, nodeCopy1, nodeCopy2);

	}
}

/*tr_datos* chooseNode(Yama* yama, tr_datos* nodeCopy1, tr_datos* nodeCopy2){

}*/
