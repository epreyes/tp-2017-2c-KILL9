/*
 * thread.c
 *
 *  Created on: 15/9/2017
 *      Author: migue tomicha
 *
 */
/**
 * Creacion de un thread
 */

#include <pthread.h>
#include <commons/log.h>
#include "thread.h"

int createThread(pthread_t * ptrThread,void * (*functionHandle) (void*),
			void* function_parameter){
	//Creacion de log
	t_log* log = log_create("Thread error","tp-share-library",false,LOG_LEVEL_ERROR);

	int result =  pthread_create(ptrThread,NULL,functionHandle,function_parameter);
	if(result != 0){
		log_error(log,"Error al crear thread");
		free(log);
	}

	return result;
}
