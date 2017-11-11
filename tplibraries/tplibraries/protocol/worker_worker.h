/*
 * node_node.h
 *
 *  Created on: 26/10/2017
 *      Author: utnso
 */

#ifndef TPLIBRARIES_PROTOCOL_WORKER_WORKER_H_
#define TPLIBRARIES_PROTOCOL_WORKER_WORKER_H_

//========NODO_A_NODO==========

typedef struct{
	char	code;			//'R'	Indica que es un hermano pidiendo tmp de RL
	char	rl_tmp[28];		//		Nombre del archivo solicitado
}nodeData_rq;


typedef struct{
	char	code;			//'R'	Respuesta a los datos del hermano
	int		fileSize;		//		Tamaño del archivo pedido
	char*	file;			//		Contenido del archivo
}nodeData_rs;

#endif /* TPLIBRARIES_PROTOCOL_WORKER_WORKER_H_ */
