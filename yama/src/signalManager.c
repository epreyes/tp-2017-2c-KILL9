/*
 * signalManager.c
 *
 *  Created on: 6/11/2017
 *      Author: utnso
 */

#include "headers/signalManager.h"

void reloadConfig(int param) {
	yama->config = getConfig();
	yama->availBase = config_get_int_value(yama->config, "NODE_AVAIL");
	yama->planningDelay = config_get_int_value(yama->config,
			"RETARDO_PLANIFICACION");
	strcpy(yama->algoritm,
			config_get_string_value(yama->config, "ALGORITMO_BALANCEO"));

	yama->debug = config_get_int_value(yama->config, "DEBUG");
	log_info(yama->log, "Se recarga la configuracion.");
}
