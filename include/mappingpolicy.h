#ifndef MAPPINGPOLICY_H
#define MAPPINGPOLICY_H

#include "computer.h"
#include "simulator.h"

typedef struct {
   unsigned tag;
   unsigned set;
   unsigned offset;
} MappingResult;

void direct_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result);
void set_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result);
void fully_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result);

#endif
