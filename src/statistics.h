#ifndef STATISTICS_H
#define STATISTICS_H

#include "gui.h"
#include "simulator.h"
#include "datastore.h"


typedef struct {
   double time;                         // The amount of time it took for the request to be solved
   long numAccesses[MAX_CACHES+1];      // The number of times each level has been accessed
   long numHits[MAX_CACHES+1];          // The number of times each level has had a hit
   long numMisses[MAX_CACHES+1];        // The number of times each level has had a miss
   long numBurstAccesses;					// The number of times there was a burst access on main memory
} Stats;

void init_statistics(Stats *stats);
void update_statistics(Computer *computer, Stats *stats);
void set_statistics(char* component, char* property, char* value);
char* get_statistics(char* component, char* property);
void print_statistics(FILE* fp);
void increment_double_statistics(char *component, char *property, double value);
void increment_integer_statistics(char *component, char *property, int value);
void calculate_rate_statistics(char *component, char *property, char *partial, char *total);

#endif
