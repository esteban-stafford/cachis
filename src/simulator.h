/**
 * @file simulator.h
 * @brief Headers and constants related to simulator.c
 */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "traceparser.h"
#include "statistics.h"

extern unsigned long cycle;

void simulate(Computer *computer);
void simulate_step(Computer *computer, MemoryOperation *operation);


typedef struct {
   int resolved;           //The cache that resolved the request
   long cacheLineDest[MAX_CACHES];      //The cache line on every cache that should contain the data
   unsigned address;       //The address from the MemoryOperation that gets executed
   unsigned size;
   unsigned *data;         //The data from the operation
} ResponseType;

#endif
