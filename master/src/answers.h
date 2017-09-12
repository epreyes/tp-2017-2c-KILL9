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
typedef struct af_datos af_datos;

/*===================TRANSFORMACION====================*/

struct tr_datos{
	int	nodo;
	char	direccion[21];
	int	bloque;
	int	tamanio;
	char	tr_tmp[28];
};

tr_datos tr_answer[] = {
	{1,"123.123.123.123:4202",68,123321,"/tmp/1415463776-T-M000-B068"},
	{1,"123.123.123.123:4202",15,123321,"/tmp/1415463777-T-M000-B015"},
	{2,"123.123.123.122:1800",87,123321,"/tmp/1415463778-T-M000-B087"},
	{2,"123.123.12.122:1800",153,123321,"/tmp/1415463779-T-M000-B153"},
	{3,"123.123.123.125:1200",18,123321,"/tmp/1415463780-T-M000-B018"},
	{4,"12.123.123.125:8081",345,123321,"/tmp/1415463781-T-M000-B345"},
};

/*===================REDUCCIÓN=LOCAL====================*/

struct rl_datos{
	int	nodo;
	char	direccion[21];
	char	tr_tmp[28];
	char	rl_tmp[24];
};

rl_datos rl_answer[] = {
	{1,"123.123.123.123:4202","/tmp/1415463776-T-M000-B068","/tmp/1415463776-RL-M000"},
	{1,"123.123.123.123:4202","/tmp/1415463777-T-M000-B015","/tmp/1415463776-RL-M000"},
	{2,"123.123.123.122:1800","/tmp/1415463778-T-M000-B087","/tmp/1415463776-RL-M000"},
	{2,"123.123.12.122:1800","/tmp/1415463780-T-M000-B018","/tmp/1415463776-RL-M000"},
	{3,"123.123.123.125:1200","/tmp/1415463780-T-M000-B018","/tmp/1415463776-RL-M000"},
	{4,"12.123.123.125:8081","/tmp/1415463781-T-M000-B345","/tmp/1415463776-RL-M000"},
};

/*===================REDUCCIÓN=GLOBAL====================*/

struct rg_datos{
	int	nodo;
	char	direccion[21];
	char	rl_tmp[24];
	char	rg_tmp[24];
	char	encargado;
};

rg_datos rg_answer[] = {
	{1,"123.123.123.123:4202","/tmp/1415463776-RL-M000","",0},
	{1,"123.123.123.123:4202","/tmp/1415463776-RL-M000","",0},
	{2,"123.123.123.122:1800","/tmp/1415463776-RL-M000","/tmp/1415463776-RG-M000",1},
	{2,"123.123.12.122:1800","/tmp/1415463776-RL-M000","",0},
	{3,"123.123.123.125:1200","/tmp/1415463776-RL-M000","",0},
	{4,"12.123.123.125:8081","/tmp/1415463776-RL-M000","",0},
};

/*===================ALMACENADO=FINAL====================*/

struct af_datos{
	int	nodo;
	char	direccion[21];
	char	rg_tmp[24];
};

af_datos af_answer[] = {
	{1,"123.123.123.123:4202","/tmp/1415463776-RG-M000"},
};

#endif /* ANSWERS_H_ */
