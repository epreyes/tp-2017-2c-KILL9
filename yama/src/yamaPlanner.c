/*
 * yamaPlanner.c
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#include "headers/yamaPlanner.h"

#include <string.h>

int getMaxWorkload(Yama* yama) {
	int max = 0;
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		if (max < elem->tasks_in_progress) {
			max = elem->tasks_in_progress;
		}
	}
	return max;
}

int clockAvail(Yama* yama) {
	return config_get_int_value(yama->config, "NODE_AVAIL");
}

int hClockAvail(Yama* yama, elem_tabla_nodos* elem) {
	return clockAvail(yama) + (getMaxWorkload(yama) - elem->tasks_in_progress);
}

int setNewAvailability(elem_tabla_nodos* elem) {

	if (strcmp(config_get_string_value(yama->config, "ALGORITMO_BALANCEO"),
			"WRR") == 0)
		return hClockAvail(yama, elem);
	if (strcmp(config_get_string_value(yama->config, "ALGORITMO_BALANCEO"),
			"RR") == 0)
		return clockAvail(yama);

	return 0;
}

int setBaseAvailability() {
	return config_get_int_value(yama->config, "NODE_AVAIL");
}

void addAvailBase() {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
		node->availability += config_get_int_value(yama->config, "NODE_AVAIL");
		list_replace(yama->tabla_nodos, index, node);
	}
}

void updateAvailability() {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		elem->availability = setNewAvailability(elem);
		list_replace(yama->tabla_nodos, index, elem);
	}

}

int getHigerAvailNode(Yama* yama) {
	elem_tabla_nodos* higer = list_get(yama->tabla_nodos, 0);
	int pos = 0;

	int index = 1;
	for (index = 1; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* current = list_get(yama->tabla_nodos, index);
		if ((current->availability > higer->availability)
				|| ((current->availability == higer->availability)
						&& (current->tasts_done < higer->tasts_done))) {
			higer = current;
			pos = index;
		}
	}

	return pos;
}

int blockExistInClock(block_info* blockRecived, int clock) {
	elem_tabla_nodos* nodo = list_get(yama->tabla_nodos, clock);
	int response = 0;

	if ((nodo->node_id == blockRecived->node1)
			|| (nodo->node_id == blockRecived->node2)) {
		response = 1;
	}
	return response;
}

tr_datos* buildNodePlaned(block_info* blockRecived, int master, int node_id) {
	tr_datos* nodeData = malloc(sizeof(tr_datos));

	if (node_id == blockRecived->node1) {
		nodeData->nodo = blockRecived->node1;
		strcpy(nodeData->ip, blockRecived->node1_ip);
		nodeData->port = blockRecived->node1_port;
		nodeData->bloque = blockRecived->node1_block;
	}
	if (node_id == blockRecived->node2) {
		nodeData->nodo = blockRecived->node2;
		strcpy(nodeData->ip, blockRecived->node2_ip);
		nodeData->port = blockRecived->node2_port;
		nodeData->bloque = blockRecived->node2_block;
	}
	nodeData->tamanio = blockRecived->end_block;
	getTmpName(nodeData, 'T', blockRecived->block_id, master);

	return nodeData;
}

void increseClock(int* clock) {
	(*clock)++;
	if ((*clock) >= list_size(yama->tabla_nodos)) {
		(*clock) = 0;
	}
}

tr_datos* updateNodeInTable(block_info* blockRecived, int master, int clock) {
	//se deberá reducir en 1 el valor de disponibilidad y avanzar el Clock al siguiente Worker
	elem_tabla_nodos* nodo = list_get(yama->tabla_nodos, clock);
	nodo->availability--;
	nodo->tasks_in_progress++;
	list_replace(yama->tabla_nodos, clock, nodo);

	//creo la fila que se va a devolver para la transformacion
	tr_datos* nodePlaned = buildNodePlaned(blockRecived, master, nodo->node_id);

	//avanzo el clock al siguiente nodo.
	increseClock(&(yama->clock));

	//devuelvo el nodo planificado.
	return nodePlaned;
}

tr_datos* evaluateClock(block_info* blockRecived, int master, int clock) {
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, clock);
	//si existe el bloque en el nodo...
	if (blockExistInClock(blockRecived, clock)) {
		//...y tiene dispo mayor a cero
		if (node->availability > 0) {
			//actualizo todo
			return updateNodeInTable(blockRecived, master, clock);
		} else {
			//si la disponibilidad es cero...
			//Si dicho clock fuera a ser asignado a un Worker cuyo nivel de disponibilidad fuera 0...
			//se deberá restaurar la disponibilidad al valor de la disponibilidad base y luego,
			//avanzar el clock al siguiente Worker, repitiendo el paso 2.
			node->availability = setBaseAvailability(node);
			list_replace(yama->tabla_nodos, clock, node);
			increseClock(&(yama->clock));
			return evaluateClock(blockRecived, master, yama->clock);
		}
	} else {
		//En el caso de que no se encuentre, se deberá utilizar el siguiente Worker que posea una disponibilidad mayor a 0.
		//Para este caso, no se deberá modificar el Worker apuntado por el Clock.
		yama->clock_aux = yama->clock;
		increseClock(&(yama->clock_aux));
		elem_tabla_nodos* nodeAux = list_get(yama->tabla_nodos,
				yama->clock_aux);
		while (nodeAux->availability == 0) {
			increseClock(&(yama->clock_aux));
			nodeAux = list_get(yama->tabla_nodos, yama->clock_aux);
		}
		if (yama->clock == yama->clock_aux) {
			//Si se volviera a caer en el Worker apuntado por el clock por no existir disponibilidad en los Workers (es decir, da una vuelta completa a los Workers),
			//se deberá sumar al valor de disponibilidad de todos los workers el valor de la Disponibilidad Base configurado al inicio de la pre-planificación.
			addAvailBase();
		}
		return evaluateClock(blockRecived, master, yama->clock_aux);
	}
}

tr_datos* doPlanning(block_info* blockRecived, int master) {

	//Se posicionará el Clock en el Worker de mayor disponibilidad, desempatando por el primer worker que tenga menor cantidad de tareas realizadas históricamente.
	yama->clock = getHigerAvailNode(yama);

	//Se deberá evaluar si el Bloque a asignar se encuentra en el Worker apuntado por el Clock y el mismo tenga disponibilidad mayor a 0.
	return evaluateClock(blockRecived, master, yama->clock);
}
