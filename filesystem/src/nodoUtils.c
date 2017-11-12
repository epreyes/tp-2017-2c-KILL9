/*
 * nodoUtils.c
 *
 *  Created on: 6/11/2017
 *      Author: utnso
 */

#include "nodoUtils.h"

// Guarda la informacion de uno o varios nodos conectados en nodos.bin
void guardarConfigNodoEnBin() {

	t_config* nodoConfig = malloc(sizeof(t_config));
	nodoConfig = config_create(fs->m_nodos);

	// Si no hay estado anterior, se guarda el nodo en nodos.bin

	if (nodoConfig == NULL) {
		int conf = config_save_in_file(nodoConfig, fs->m_nodos);
	}

	// Recorro la lista nodos y refresco la config

	int i = 0;
	int totalTamanio = 0;
	int totalLibre = 0;
	char* listaNodos = string_new();
	int cantNodos = list_size(nodos->nodos);
	string_append(&listaNodos, "[");
	for (i = 0; i < cantNodos; i++) {

		t_nodo* nod = list_get(nodos->nodos, i);

		string_append(&listaNodos, "Nodo");
		string_append(&listaNodos, string_itoa(nod->id));

		if (cantNodos == 1)
			string_append(&listaNodos, "]");

		if (cantNodos > 1 && i < (cantNodos - 1))
			string_append(&listaNodos, ",");

		totalTamanio += nod->total;
		totalLibre += nod->libre;

		// Creo la entrada del nuevo nodo

		char* nodoTotalProperty = string_new();
		string_append(&nodoTotalProperty, "Nodo");
		string_append(&nodoTotalProperty, string_itoa(nod->id));
		string_append(&nodoTotalProperty, "Total");

		config_set_value(nodoConfig, nodoTotalProperty,
				string_itoa(nod->total));

		free(nodoTotalProperty);

		char* nodoLibreProperty = string_new();
		string_append(&nodoLibreProperty, "Nodo");
		string_append(&nodoLibreProperty, string_itoa(nod->id));
		string_append(&nodoLibreProperty, "Libre");

		config_set_value(nodoConfig, nodoLibreProperty,
				string_itoa(nod->libre));

		free(nodoLibreProperty);

	}

	if (cantNodos > 1)
		string_append(&listaNodos, "]");

	// Actualizo totales
	config_set_value(nodoConfig, "TAMANIO", string_itoa(totalTamanio));
	config_set_value(nodoConfig, "LIBRE", string_itoa(totalLibre));

	// Actualizo lista NODOS
	config_set_value(nodoConfig, "NODOS", listaNodos);

	config_save(nodoConfig);

	free(listaNodos);

}

// Actualiza nodos.bin con la informacion de los nodos modificados y el total general
void actualizarConfigNodoEnBin(t_nodo* nodo) {
	t_config* nodoConfig = malloc(sizeof(t_config));

	nodoConfig = config_create(fs->m_nodos);

	char* nodoLibre = string_new();
	string_append(&nodoLibre, "Nodo");
	string_append(&nodoLibre, string_itoa(nodo->id));
	string_append(&nodoLibre, "Libre");


	if (config_has_property(nodoConfig, nodoLibre)) {

		config_set_value(nodoConfig, nodoLibre, string_itoa(nodo->libre));

		// Actualizo totales
		if (config_has_property(nodoConfig, "LIBRE")) {
			int libreTotal = config_get_int_value(nodoConfig, "LIBRE");
			libreTotal -= 1;
			config_set_value(nodoConfig, "LIBRE", string_itoa(libreTotal));
		}

		config_save(nodoConfig);
	}

	free(nodoLibre);
	config_destroy(nodoConfig);

}

// Indica si un nodo que se conecta pertenece a un estado anterior y devuelve su info
t_nodo* nodoPerteneceAEstadoAnterior(int idNodo) {

	t_config* nodoConfig = malloc(sizeof(t_config));

	nodoConfig = config_create(fs->m_nodos);

	char* nodoTotal = string_new();
	string_append(&nodoTotal, "Nodo");
	string_append(&nodoTotal, string_itoa(idNodo));
	string_append(&nodoTotal, "Total");

	char* nodoLibre = string_new();
	string_append(&nodoLibre, "Nodo");
	string_append(&nodoLibre, string_itoa(idNodo));
	string_append(&nodoLibre, "Libre");

	t_nodo* nodAnt = malloc(sizeof(t_nodo));

	if (config_has_property(nodoConfig, nodoTotal))
		nodAnt->total = config_get_int_value(nodoConfig, nodoTotal);

	if (config_has_property(nodoConfig, nodoLibre))
		nodAnt->libre = config_get_int_value(nodoConfig, nodoLibre);

	if (!config_has_property(nodoConfig, nodoTotal)
			|| !config_has_property(nodoConfig, nodoTotal)) {
		free(nodAnt);
		free(nodoLibre);
		free(nodoTotal);
		config_destroy(nodoConfig);
		return NULL;
	}

	free(nodoLibre);
	free(nodoTotal);
	config_destroy(nodoConfig);

	return nodAnt;
}

int buscarNodoPorSocket(int socketNodo) {
	int i = 0;
	for (i = 0; i < list_size(nodos->nodos); i++) {
		t_nodo* nod = list_get(nodos->nodos, i);
		if (nod->socketNodo == socketNodo)
			return nod->id;
	}
	return -1;

}

// Determina si existe un nodo
int buscarNodoPorId(int idNodo) {
	int i = 0;
	for (i = 0; i < list_size(nodos->nodos); i++) {
		t_nodo* nod = list_get(nodos->nodos, i);
		if (nod->id == idNodo)
			return nod->id;
	}
	return -1;

}

// Devuelve la info de un nodo por id de nodo
t_nodo* buscarNodoPorId_(int idNodo) {
	int i = 0;
	for (i = 0; i < list_size(nodos->nodos); i++) {
		t_nodo* nod = list_get(nodos->nodos, i);
		if (nod->id == idNodo)
			return nod;
	}
	return NULL;

}
