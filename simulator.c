#include <stdio.h>
#include <string.h>
#include <math.h>
#include "confparser.h"
#include "datainterface.h"
#include "datamanipulation.h"
#include "simulator.h"

unsigned long cycle = 0;

/* Private functions */
void incrementDoubleStatistics(char *component, char *property, double value);
void incrementIntegerStatistics(char *component, char *property, int value);
void calculateRateStatistics(char *component, char *property, char *partial, char *total);

void simulate() {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(&memoryOperations[i]);
   }
}

void simulate_step(struct memOperation *operation) {
}

void incrementDoubleStatistics(char *component, char *property, double value) {
   double oldValue = 0.0;
   char *oldValueString = getStatistics(component,property);
   if(oldValueString) 
      oldValue = strtod(oldValueString, NULL);
   char tmp[20];
   sprintf(tmp, "%lf", oldValue+value);
   setStatistics(component, property, tmp);
}

void incrementIntegerStatistics(char *component, char *property, int value) {
   int oldValue = 0.0;
   char *oldValueString = getStatistics(component,property);
   if(oldValueString) 
      oldValue = atoi(oldValueString);
   char tmp[20];
   sprintf(tmp, "%d", oldValue+value);
   setStatistics(component, property, tmp);
}

void calculateRateStatistics(char *component, char *property, char *partialName, char *totalName) {
   double partial = 0.0;
   double total = 0.0;
   char *valueString = getStatistics(component,partialName);
   if(valueString) 
      partial = strtod(valueString, NULL);
   valueString = getStatistics(component,totalName);
   if(valueString) 
      total = strtod(valueString, NULL);
   char tmp[20] = "NaN";
   if(total != 0) {
      sprintf(tmp, "%0.2lf", partial/total);
   }
   setStatistics(component, property, tmp);
}

