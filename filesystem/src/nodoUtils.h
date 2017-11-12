/*
 * nodoUtils.h
 *
 *  Created on: 6/11/2017
 *      Author: utnso
 */

#ifndef NODOUTILS_H_
#define NODOUTILS_H_

#include "fs.h"

void actualizarConfigNodoEnBin(t_nodo* nodo);
t_nodo* nodoPerteneceAEstadoAnterior(int idNodo);
void guardarConfigNodoEnBin();
int buscarNodoPorSocket(int socketNodo);
int buscarNodoPorId(int idNodo);
t_nodo* buscarNodoPorId_(int idNodo);

#endif /* NODOUTILS_H_ */
