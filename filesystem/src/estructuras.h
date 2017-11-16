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
		int size;
	} t_header;

	typedef struct {
		char* idNodo0;
		char* idNodo1;
		int idBloque0;
		int idBloque1;
		long finBytes;
		int nroBloque;
		char* dirWorker0;
		char* dirWorker1;
	} t_bloqueInfo;

	typedef struct {
		int handshake;
		int block_quantity;
		int id_nodo;
		int data_size;
		char* worker_ip_port;
	} t_infoNodo;

// Errores escritura

#define SIN_ESPACIO 200
#define ARCHIVO_EXISTENTE 201
#define SIN_CONTENIDO_ESCRIBIR 202
#define NO_HAY_NODOS 203
#define TAMANIO_RENGLON_INVALIDO 204
#define NO_EXISTE_DIR_DESTINO 205
#define FS_SIN_FORMATO 206
#define ERROR_MISMO_NODO_COPIA 207


#endif /* ESTRUCTURAS_H_ */

