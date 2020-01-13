#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "traceparser.h"

extern unsigned long cycle;

void simulate();
void simulate_step(struct memOperation *operation);

#endif
