/*
 * apareo.h
 *
 *  Created on: 28/10/2017
 *      Author: utnso
 */

#ifndef HEADERS_APAREO_H_
#define HEADERS_APAREO_H_

#include <stdio.h>
#include <string.h>


char *minorWord(char* word1, char* word2);

void readWord(char *word, FILE *file, int* endFile);

void mergeFiles(char* Aname,char* Bname, char *Cname);


#endif /* HEADERS_APAREO_H_ */
