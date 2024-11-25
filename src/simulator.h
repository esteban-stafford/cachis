#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "traceparser.h"

extern unsigned long cycle;

void simulate(Computer *computer);
void simulate_step(Computer *computer, MemoryOperation *operation);


typedef struct {
   double time;            //The amount of time it took for the request to be solved
   int resolved;           //The cache that resolved the request
   unsigned address;       //The address from the MemoryOperation that gets executed
   unsigned size;
   unsigned *data;         //The data from the operation
} ResponseType;

#endif
