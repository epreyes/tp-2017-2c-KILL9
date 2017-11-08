/*
 * signalManager.c
 *
 *  Created on: 6/11/2017
 *      Author: utnso
 */

#include "headers/signalManager.h"

void reloadConfig(int param){
	//cargar el archivo de configuracion en memoria.
	//editar los datos correspondientes.
	//guardar los cambios.
	printf("\nCapture la señan!\n");
	yama->config = getConfig();
	viewConfig();
}
