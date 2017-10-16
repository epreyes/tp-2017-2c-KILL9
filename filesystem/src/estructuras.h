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
		int finBytes;
	// Agregar ip y puerto
	} t_bloqueInfo;

#define YAMA_HSK 1001
#define NODO_HSK 1002

#define YAMA_ARCHIVO_INFO 2001
#define ERROR1 2002
#define RESPUESTA_OK 2003

// Errores

#define SIN_ESPACIO 200
#define ARCHIVO_EXISTENTE 201
#define SIN_CONTENIDO_ESCRIBIR 202

#endif /* ESTRUCTURAS_H_ */
