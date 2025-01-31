/**
 * @file confparser.h
 * @brief Headers and constants related to confparser.c
 */

#ifndef CONFPARSER_H
#define CONFPARSER_H

#include "iniparser.h"
#include "computer.h"

#define N_BITS_MASK 9
#define NCLAVES_CPU 5
#define NCLAVES_MEMORY 5
#define NCLAVES_CACHE 8


dictionary *readConfigurationFile(char * file);
int parseConfiguration(dictionary *ini, Computer *computer);
void checkSectionKeys(dictionary *ini, const char *section, int numberOfValidKeys, char *validKeys[], int *errors);
void showConfiguration(Computer *computer);

#endif
