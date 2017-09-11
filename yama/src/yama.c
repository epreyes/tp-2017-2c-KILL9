/*
 ============================================================================
 Name        : yama.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "yama.h"

/*---------------------- Private ---------------------------------*/
t_config* getConfig(){
	t_config* config = config_create(CONFIG_PATH);
	return config;
}

/*---------------------- Public ----------------------------------*/
Yama createYama(){
	Yama yama;
	t_config* config = getConfig();

	printf("\nConfig archivo: %s\n", config->path);

	yama.port = config_get_int_value(config, "YAMA_PUERTO");
	yama.fs_port = config_get_int_value(config, "FS_PUERTO");
	yama.planning_delay = config_get_int_value(config, "RETARDO_PLANIFICACION");

	strcpy(yama.fs_ip, config_get_string_value(config, "FS_IP"));
	strcpy(yama.balancign_algoritm, config_get_string_value(config, "ALGORITMO_BALANCEO"));

	return yama;
}
