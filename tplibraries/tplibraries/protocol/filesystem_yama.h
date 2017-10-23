/*
 * filesystem_yama.h
 *
 *  Created on: 14/10/2017
 *      Author: utnso
 */

#ifndef TPLIBRARIES_PROTOCOL_FILESYSTEM_YAMA_H_
#define TPLIBRARIES_PROTOCOL_FILESYSTEM_YAMA_H_

typedef struct {
		char idMensaje; //T
		int size;
		char* fileName;
	} yama_rq;

	//antes viene la cantidad de bloques.
	typedef struct {
		int block_id;
		int node1;
		char node1_ip[15];
		int node1_port;
		int node1_block;

		int node2;
		char node2_ip[15];
		int node2_port;
		int node2_block;
		int end_block;
	}block_info;

#endif /* TPLIBRARIES_PROTOCOL_FILESYSTEM_YAMA_H_ */
