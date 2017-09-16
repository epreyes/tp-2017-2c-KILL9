/*
 * utils.c
 *
 *  Created on: 12/09/2017
 *      Author: utnso
 */

#include "utils.h"

int fileExist(char *file){
	FILE* fparchivo= fopen (file, "r");
	return (fparchivo != NULL);
}
