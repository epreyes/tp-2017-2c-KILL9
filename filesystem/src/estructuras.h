/*
 * estructuras.h
 *
 *  Created on: 15/10/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

// Mensajeria (poner en archivo comun)

// Header
typedef struct
	__attribute__((packed)) {
		char idMensaje;
		long size;
	} t_header;

	typedef struct {
		char* idNodo0;
		char* idNodo1;
		int idBloque0;
		int idBloque1;
		long finBytes;
	// Agregar ip y puerto
	} t_bloqueInfo;

// Errores

#define SIN_ESPACIO 200
#define ARCHIVO_EXISTENTE 201
#define SIN_CONTENIDO_ESCRIBIR 202

#endif /* ESTRUCTURAS_H_ */
