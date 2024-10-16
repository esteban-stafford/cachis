#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "traceparser.h"

extern unsigned long cycle;

void simulate(Computer *computer);
void simulate_step(Computer *computer, struct memOperation *operation);

#endif
