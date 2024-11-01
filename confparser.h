#ifndef CONFPARSER_H
#define CONFPARSER_H

#include "iniparser.h"
#include "computer.h"

#define N_BITS_MASK 9

dictionary *readConfigurationFile(char * file);
int parseConfiguration(dictionary *ini, Computer *computer);
void checkSectionKeys(dictionary *ini, const char *section, int numberOfValidKeys, char *validKeys[], int *errors);
void showConfiguration(Computer *computer);

#endif
