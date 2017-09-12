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
	int		nodo;
	char	direccion[21];
	int		bloque;
	int		tamanio;
	char	tr_tmp[28];
};

tr_datos tr_answer[] = {
	{1,"123.123.123.123:4202",68,100000,"/tmp/1415463776-T-M000-B068"},
	{1,"123.123.123.123:4202",15,123321,"/tmp/1415463777-T-M000-B015"},
	{2,"123.123.123.122:1800",87,123321,"/tmp/1415463778-T-M000-B087"},
	{2,"123.123.12.122:1800",153,123321,"/tmp/1415463779-T-M000-B153"},
	{3,"123.123.123.125:1200",18,300000,"/tmp/1415463780-T-M000-B018"},
	{4,"12.123.123.125:8081",345,123321,"/tmp/1415463781-T-M000-B345"},
	{4,"12.123.123.125:8081",23,123321,"/tmp/1415463781-T-M000-B023"},
	{4,"12.123.123.125:8081",45,123321,"/tmp/1415463781-T-M000-B045"},
	{4,"12.123.123.125:8081",324,123321,"/tmp/1415463781-T-M000-B324"},
	{4,"12.123.123.125:8081",13,123321,"/tmp/1415463781-T-M000-B013"},
};

/*===================REDUCCIÓN=LOCAL====================*/

struct rl_datos{
	int	nodo;
	char	direccion[21];
	char	tr_tmp[28];
	char	rl_tmp[28];
};

rl_datos rl_answer[] = {
	{1,"123.123.123.123:4202","/tmp/1415463776-T-M000-B068","/tmp/1415463776-RL-M000-W01"},
	{1,"123.123.123.123:4202","/tmp/1415463777-T-M000-B015","/tmp/1415463776-RL-M000-W01"},
	{2,"123.123.123.122:1800","/tmp/1415463778-T-M000-B087","/tmp/1415463776-RL-M000-W02"},
	{2,"123.123.12.122:1800","/tmp/1415463780-T-M000-B018","/tmp/1415463776-RL-M000-W02"},
	{3,"123.123.123.125:1200","/tmp/1415463780-T-M000-B018","/tmp/1415463776-RL-M000-W03"},
	{4,"12.123.123.125:8081","/tmp/1415463781-T-M000-B345","/tmp/1415463776-RL-M000-W04"},
};

/*===================REDUCCIÓN=GLOBAL====================*/

struct rg_datos{				//es un record por nodo
	int		nodo;
	char	direccion[21];
	char	rl_tmp[28];
	char	rg_tmp[24];
	char	encargado;
};

rg_datos rg_answer[] = {
	{1,"123.123.123.123:4202","/tmp/1415463776-RL-M000-W01","",0},
	{2,"123.123.123.122:1800","/tmp/1415463776-RL-M000-W02","/tmp/1415463776-RG-M000",1},
	{3,"123.123.123.125:1200","/tmp/1415463776-RL-M000-W03","",0},
	{4,"12.123.123.125:8081","/tmp/1415463776-RL-M000-W04","",0},
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
