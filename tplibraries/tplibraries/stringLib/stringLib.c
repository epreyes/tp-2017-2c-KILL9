/*
 * stringLib.c
 *
 *  Created on: 13/09/2017
 *      Author: utnso
 */

#include "stringLib.h"

void append_int(char* char1, int value){
	char* s_value = string_itoa(value);
	string_append(&char1, s_value);
}
