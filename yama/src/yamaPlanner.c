/*
 * yamaPlanner.c
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#include "headers/yamaPlanner.h"

#include <string.h>

int getMaxWorkload(Yama* yama){
	int max = 0;
	int index = 0;
	for(index = 0; index < list_size(yama->tabla_nodos); index++){
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		if( max < elem->tasks_in_progress ){
			max = elem->tasks_in_progress;
		}
	}
	return max;
}

int clockAvail(Yama* yama){
	return config_get_int_value(yama->config, "NODE_AVAIL");
}

int hClockAvail(Yama* yama, elem_tabla_nodos* elem){
	return clockAvail(yama) + (getMaxWorkload(yama) - elem->tasks_in_progress);
}

int setNewAvailability(Yama* yama, elem_tabla_nodos* elem){

	if (strcmp(config_get_string_value(yama->config, "ALGORITMO_BALANCEO"), "WRR") == 0)
			return hClockAvail(yama, elem);
	if (strcmp(config_get_string_value(yama->config, "ALGORITMO_BALANCEO"), "RR") == 0)
			return clockAvail(yama);

	return 0;
}

void updateAvailability(Yama* yama){
	int index = 0;
	for(index = 0; index < list_size(yama->tabla_nodos); index++){
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		elem->availability = setNewAvailability(yama, elem);
		list_replace(yama->tabla_nodos, index, elem);
	}


}

elem_tabla_nodos* getHigerAvailNode(Yama* yama){
	elem_tabla_nodos* higer = list_get(yama->tabla_nodos, 0);

	int index = 1;
	for(index = 1; index < list_size(yama->tabla_nodos); index++){
		elem_tabla_nodos* current = list_get(yama->tabla_nodos, index);
		if( (current->availability > higer->availability) ||
				((current->availability == higer->availability) && (current->tasts_done < higer->tasts_done)) ){
			higer = current;
		}
	}

	return higer;
}

void doPlanning(Yama* yama, block* blockRecived) {
	updateAvailability(yama);
	elem_tabla_nodos* clock = getHigerAvailNode(yama);
	//blockRecived->
}
