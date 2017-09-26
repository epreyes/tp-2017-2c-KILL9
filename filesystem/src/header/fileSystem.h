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
#include <protocol/protocol.h>
#include <commons/log.h>
#include <pthread.h>

#define DATANODE 1
#define MEMORIA 2
#define	SOCKET_BACKLOG   50

/***********************************************
 ************ Variables globales ***************
 ***********************************************/
t_config * config;
t_file_system_config* file_system_config;

/** log **/
t_log* infoLogger;
t_log* errorLogger;

int listenning_socket;
uint32_t vg_tamanio_memoria, vg_tamanio_marco, vg_cant_paginas_adm,
		vg_cant_marcos;
int vg_paginas_stack;

typedef enum {
	ACCESOAPROBADO = 0, ACCESODENEGADO = 1
} handshake;

typedef enum {
	LEER = 1, ESCRIBIR = 2
} tipoCpu;


/** Funciones **/
void load_file_system_properties(void);

int process_request(void * socket);

void loadServidorMultihread(void);

void process_dataNode();

#endif /* HEADER_FILESYSTEM_H_ */
