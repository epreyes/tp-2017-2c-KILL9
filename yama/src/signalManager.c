/*
 * signalManager.c
 *
 *  Created on: 6/11/2017
 *      Author: utnso
 */

#include "headers/signalManager.h"

void reloadConfig(int param){
	yama->config = getConfig();
	log_info(yama->log, "Se recarga la configuracion.");
}
