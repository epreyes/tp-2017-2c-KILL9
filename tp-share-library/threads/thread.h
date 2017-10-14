/*
 * thread.h
 *
 *  Created on: 15/9/2017
 *      Author: miguel tomicha
 */



#ifndef THREADS_THREAD_H_
#define THREADS_THREAD_H_


/**
 * Manejo de todo lo  relacionado con multithread
 */

int createThread(pthread_t * ptrThread,void * (*functionHandle) (void*),
			void* function_parameter);

#endif /* THREADS_THREAD_H_ */
