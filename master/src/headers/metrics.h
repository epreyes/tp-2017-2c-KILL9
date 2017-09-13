/*
 * metrics.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef HEADERS_METRICS_H_
#define HEADERS_METRICS_H_

#include "master.h"
#include "utils.h"

//=======Estructuras para las métricas=======//

metrics masterMetrics;

//==============Funciones====================//

void printSeparator(char c);

void printProcessMetrics(char* name, procMetrics metrics);

void printMetrics(metrics masterMetrics);

#endif /* HEADERS_METRICS_H_ */
