/*
 * globalReduction.h
 *
 *  Created on: 12/10/2017
 *      Author: utnso
 */

#ifndef SRC_HEADERS_GLOBALREDUCTION_H_
#define SRC_HEADERS_GLOBALREDUCTION_H_

#include "yama.h"

void* processGlobalReduction(int master);
void getGlobalReductionTmpName(rg_datos* nodeData, int op, int blockId, int masterId);


#endif /* SRC_HEADERS_GLOBALREDUCTION_H_ */
