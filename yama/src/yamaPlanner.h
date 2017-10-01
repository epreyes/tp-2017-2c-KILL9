/*
 * yamaPlanner.h
 *
 *  Created on: 25/09/2017
 *      Author: utnso
 */

#ifndef YAMAPLANNER_H_
#define YAMAPLANNER_H_

#include "yama.h"
#include <commons/collections/list.h>

void doPlanning(Yama* yama, t_list* nodeList);

void clockPlanning(Yama* yama, t_list* nodeList);

void hClockPlanning(Yama* yama, t_list* nodeList);

#endif /* YAMAPLANNER_H_ */
