/*
 * utils.h
 *
 *  Created on: 12/9/2017
 *      Author: utnso
 */

#ifndef HEADERS_UTILS_H_
#define HEADERS_UTILS_H_

#include "master.h"

double timediff(struct timeval *a, struct timeval *b);

void validateArgs(int argc, char* argv[]);

void loadScripts(char* transformScript, char* reductionScript);

void loadConfigs(void);

void createLoggers(void);

#endif /* HEADERS_UTILS_H_ */
