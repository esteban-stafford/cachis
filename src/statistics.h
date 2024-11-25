#ifndef STATISTICS_H
#define STATISTICS_H

#include "gui.h"
#include "datastore.h"

void set_statistics(char* component, char* property, char* value);
char* get_statistics(char* component, char* property);
void print_statistics(FILE* fp);

#endif
