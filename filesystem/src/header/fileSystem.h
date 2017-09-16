/*
 * header.h
 *
 *  Created on: 16/9/2017
 *      Author: utnso
 */

#ifndef HEADER_FILESYSTEM_H_
#define HEADER_FILESYSTEM_H_


#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <struct/struct.h>
#define	SOCKET_BACKLOG   50

/** Variables globales **/
t_config * config;
t_file_system_config* file_system_config;
int listenning_socket;

/** Funciones **/
void load_file_system_properties(void);

#endif /* HEADER_FILESYSTEM_H_ */
