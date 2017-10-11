/*
 * protocol.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

/*========TRANSFORM==========*/
typedef struct{
	char	code;
	size_t	fileNameSize;
	char*	fileName;
}transform_rq;

/*========LOCAL_RED==========*/
typedef struct{
	char	code;
}local_rq;

/*=======GLOBAL_RED==========*/
typedef struct{
	char	code;
}global_rq;

/*=======FINAL_STORAGE==========*/
typedef struct{
	char	code;
}store_rq;

#endif /* PROTOCOL_H_ */
