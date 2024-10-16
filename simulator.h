#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "traceparser.h"

extern unsigned long cycle;

void simulate(struct structComputer *computer);
void simulate_step(struct structComputer *computer, struct memOperation *operation);

#endif
