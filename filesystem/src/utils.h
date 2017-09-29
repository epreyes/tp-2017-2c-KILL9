/*
 * utils.h
 *
 *  Created on: 13/09/2017
 *      Author: epreyes
 */

#ifndef UTILSSOPORTE_H_
#define UTILSSOPORTE_H_


void listarDirectorios(t_directorio* dir);
void simularEntradaConexionNodo(t_nodos* nodos);

int obtenerProfDir(char* path);
char** obtenerDirectorios(char* path);

#endif /* UTILS_H_ */
