/*
 * answers.h
 *
 *  Created on: 9/9/2017
 *      Author: utnso
 */

#ifndef ANSWERS_H_
#define ANSWERS_H_

typedef struct tr_datos tr_datos;
typedef struct rl_datos rl_datos;
typedef struct rg_datos rg_datos;

/*===================TRANSFORMACION====================*/

struct tr_datos{
	int		nodo;
	char	direccion[21];
	int		bloque;
	int		tamanio;
	char	tr_tmp[10];
};

tr_datos tr_answer[] = {
	{1,"123.123.123.123:4202",68,123321,"tmp1"},
	{1,"123.123.123.123:4202",15,123321,"tmp2"},
	{2,"123.123.123.122:1800",87,123321,"tmp3"},
	{2,"123.123.12.122:1800",153,123321,"tmp4"},
	{3,"123.123.123.125:1200",18,123321,"tmp5"},
	{4,"12.123.123.125:8081",345,123321,"tmp6"},
};

/*===================REDUCCIÓN=LOCAL====================*/

struct rl_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[10];
	char	rl_tmp[10];
};

rl_datos rl_answer[] = {
	{1,"123.123.123.123:4202","tmp2","tmp3"},
	{1,"123.123.123.123:4202","tmp2","tmp2"},
	{2,"123.123.123.122:1800","tmp2","tmp3"},
	{2,"123.123.12.122:1800","tmp2","tmp4"},
	{3,"123.123.123.125:1200","tmp2","tmp5"},
	{4,"12.123.123.125:8081","tmp2","tmp6"},
};

/*===================REDUCCIÓN=GLOBAL====================*/

struct rg_datos{
	int		nodo;
	char	direccion[21];
	char	tr_tmp[10];
	char	rl_tmp[10];
	char	encargado;
};

rg_datos rg_answer[] = {
	{1,"123.123.123.123:4202","tmp2","tmp2",0},
	{1,"123.123.123.123:4202","tmp2","tmp2",0},
	{2,"123.123.123.122:1800","tmp2","tmp2",1},
	{2,"123.123.12.122:1800","tmp2","tmp2",1},
	{3,"123.123.123.125:1200","tmp2","tmp2",0},
	{4,"12.123.123.125:8081","tmp2","tmp2",0},
};

#endif /* ANSWERS_H_ */
