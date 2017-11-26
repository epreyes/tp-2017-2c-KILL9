/*
 * yamaPlanner.c
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#include "headers/planner.h"
#include "headers/tablesManager.h"
#include "headers/transformation.h"
#include <string.h>
#include <unistd.h>

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

int hClockAvail(Yama* yama, elem_tabla_nodos* elem, int avail) {
	return avail + (getMaxWorkload(yama) - elem->tasks_in_progress);
}

int setNewAvailability(elem_tabla_nodos* elem, char* algoritm, int avail) {

	if (strcmp(algoritm, "WRR") == 0)
		return hClockAvail(yama, elem, avail);
	if (strcmp(algoritm, "RR") == 0)
		return avail;

	return 0;
}

void addAvailBase(int avail) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* node = list_get(yama->tabla_nodos, index);
		node->availability += avail;
		list_replace(yama->tabla_nodos, index, node);
	}
}

void updateAvailability(char* algoritm, int avail) {
	int index = 0;
	for (index = 0; index < list_size(yama->tabla_nodos); index++) {
		elem_tabla_nodos* elem = list_get(yama->tabla_nodos, index);
		elem->availability = setNewAvailability(elem, algoritm, avail);
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

int increseClock(int clock) {
	clock++;
	if (clock >= list_size(yama->tabla_nodos)) {
		clock = 0;
	}
	return clock;
}

tr_datos* updateNodeInTable(block_info* blockRecived, int master, int clock) {
	//se deberá reducir en 1 el valor de disponibilidad y avanzar el Clock al siguiente Worker
	elem_tabla_nodos* nodo = list_get(yama->tabla_nodos, clock);
	increaseNodeCharge(nodo->node_id);

	//creo la fila que se va a devolver para la transformacion
	tr_datos* nodePlaned = buildNodePlaned(blockRecived, master, nodo->node_id);

	//avanzo el clock al siguiente nodo.
	yama->clock = increseClock(yama->clock);

	if (yama->debug) {
		log_info(yama->log, "Se selecciona el Nodo %d", nodo->node_id);
		log_info(yama->log,
				"--------------- Fin Planificacion ---------------");
		viewNodeTable();
	}

	//devuelvo el nodo planificado.
	return nodePlaned;
}

tr_datos* evaluateClock(block_info* blockRecived, int master, int clock,
		t_planningParams* planningParams) {
	elem_tabla_nodos* node = list_get(yama->tabla_nodos, clock);
	//si existe el bloque en el nodo...
	if ( node->errors == 0 && blockExistInClock(blockRecived, clock)) {
		if (yama->debug == 1) {
			log_info(yama->log, "Bloque %d existe en Clock = Nodo %d",
					blockRecived->block_id, node->node_id);
		}
		//...y tiene dispo mayor a cero
		if (node->availability > 0) {
			if (yama->debug == 1) {
				log_info(yama->log, "Clock = Nodo %d tiene disponibilidad %d",
						node->node_id, node->availability);
			}
			//actualizo todo
			return updateNodeInTable(blockRecived, master, clock);
		} else {
			//si la disponibilidad es cero...
			//Si dicho clock fuera a ser asignado a un Worker cuyo nivel de disponibilidad fuera 0...
			//se deberá restaurar la disponibilidad al valor de la disponibilidad base y luego,
			//avanzar el clock al siguiente Worker, repitiendo el paso 2.
			if (yama->debug == 1) {
				log_info(yama->log, "Clock = Nodo %d tiene disponibilidad %d",
						node->node_id, node->availability);
			}
			node->availability = planningParams->availBase;

			if (yama->debug == 1) {
				log_info(yama->log,
						"Se restaura la disponibilidad base (%d) al Nodo %d",
						config_get_int_value(yama->config, "NODE_AVAIL"),
						node->node_id);
			}

			list_replace(yama->tabla_nodos, clock, node);
			yama->clock = increseClock(yama->clock);

			if (yama->debug == 1) {
				elem_tabla_nodos* newNode = list_get(yama->tabla_nodos,
						yama->clock);

				log_info(yama->log,
						"Se incrementa el Clock. Nuevo Valor = Nodo %d y se vuelve a evaluar.",
						newNode->node_id);
			}

			return evaluateClock(blockRecived, master, yama->clock,
					planningParams);
		}
	} else {
		//En el caso de que no se encuentre, se deberá utilizar el siguiente Worker que posea una disponibilidad mayor a 0.
		//Para este caso, no se deberá modificar el Worker apuntado por el Clock.
		yama->clock_aux = increseClock(yama->clock_aux);

		if (yama->debug == 1) {
			elem_tabla_nodos* newNode = list_get(yama->tabla_nodos,
					yama->clock_aux);
			log_info(yama->log,
					"El bloque no se encuentra en el Clock. Se incrementa. Nuevo Valor = Nodo %d y se vuelve a evaluar.",
					newNode->node_id);
		}

		elem_tabla_nodos* nodeAux = list_get(yama->tabla_nodos,
				yama->clock_aux);
		while (nodeAux->availability == 0 && yama->clock != yama->clock_aux) {
			yama->clock_aux = increseClock(yama->clock_aux);
			nodeAux = list_get(yama->tabla_nodos, yama->clock_aux);
		}
		if (yama->clock == yama->clock_aux) {
			//Si se volviera a caer en el Worker apuntado por el clock por no existir disponibilidad en los Workers (es decir, da una vuelta completa a los Workers),
			//se deberá sumar al valor de disponibilidad de todos los workers el valor de la Disponibilidad Base configurado al inicio de la pre-planificación.
			addAvailBase(planningParams->availBase);

			if (yama->debug == 1) {
				log_info(yama->log,
						"No existe disponibilidad en ningun nodo. Se suma la disponibilidad base %d",
						config_get_int_value(yama->config, "NODE_AVAIL"));
			}
		}
		return evaluateClock(blockRecived, master, yama->clock_aux,
				planningParams);
	}
}

tr_datos* doPlanning(block_info* blockRecived, int master,
		t_planningParams* planningParams) {

	//Se posicionará el Clock en el Worker de mayor disponibilidad, desempatando por el primer worker que tenga menor cantidad de tareas realizadas históricamente.
	yama->clock = getHigerAvailNode(yama);
	elem_tabla_nodos* nodoClock = list_get(yama->tabla_nodos, yama->clock);
	yama->clock_aux = yama->clock;
	//Se deberá evaluar si el Bloque a asignar se encuentra en el Worker apuntado por el Clock y el mismo tenga disponibilidad mayor a 0.
	int planigDelay = planningParams->planningDelay;

	if (yama->debug == 1) {
		log_info(yama->log,
				"----------------- Planificacion -----------------");
		log_info(yama->log, "Clock = Nodo %d", nodoClock->node_id);
		log_info(yama->log, "Retardo = %d ms", planigDelay);
	}

	long planingDelay = planningParams->planningDelay * 1000;
	usleep(planingDelay);

	return evaluateClock(blockRecived, master, yama->clock, planningParams);
}

void* replanTask(int master, int node, t_planningParams* params, t_job* job,
		int jobindex) {
	if (job->replanificaciones == 1) {
		log_error(yama->log, "No se puede replanificar el nodo %d, Job %d.",
				node, job->id);
		job->replanificaciones++;
		job->estado = 'E';
		list_replace(yama->tabla_jobs, jobindex, job);
		return abortJob(master, node, 'T', job);
	} else {
		t_list* taskFailed = getTaskFailed(master, node, job);
		t_list* replanedTasks = list_create();
		char* filename = malloc(sizeof(char) * 28);

		int index = 0;
		for (index = 0; index < list_size(taskFailed); index++) {
			elem_tabla_estados* elem = list_get(taskFailed, index);
			strcpy(filename, elem->fileProcess);
			block_info* blockInfo = findBlock(elem->block);
			int node = elem->node;

			tr_datos* dataNode = doPlanning(blockInfo, master, params);

			setInStatusTable(job->id, 'T', master, dataNode->nodo,
					getBlockId(dataNode->tr_tmp), dataNode->tr_tmp,
					dataNode->bloque, elem->fileProcess);

			list_add(replanedTasks, dataNode);
		}

		job->replanificaciones = 1;
		list_replace(yama->tabla_jobs, jobindex, job);

		return sortTransformationResponse(replanedTasks, master, filename, job);
	}
}
