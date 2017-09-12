/*
 * protocol.h
 *
 *  Created on: 11/09/2017
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

typedef struct{
	int op;
	int msg_size;
}header;

typedef struct{

}transformationMsg;

typedef struct{

}localReductionMsg;

typedef struct{

}globalReductionMsg;

typedef struct{

}finalStoreMsg;
#endif /* PROTOCOL_H_ */
