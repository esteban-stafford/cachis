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
int selectVia(int instructionOrData, int cacheLevel, int set);

struct response_type {
   double time;
   int resolved;
   unsigned address;
   unsigned size;
   unsigned *data;
};

void simulate() {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(&memoryOperations[i]);
   }
}

void simulate_step(struct memOperation *operation) {
   struct response_type response;
   char cacheName[20];
   response.size = operation->size/4;
   response.address = operation->address;
   response.time = 0.0;
   response.data = malloc((sizeof(unsigned)));
   response.data[0] = operation->data;
   response.resolved = -1;

   printf("Simulating operation: ");
   printMemOperation(stdout, operation);
   incrementIntegerStatistics("CPU", "Accesses", 1);
 
   // Iterate through all the cache levels
   for(int cacheLevel = 0; cacheLevel < numberCaches; cacheLevel++){
      sprintf(cacheName,"Cache L%d",cacheLevel+1);
      // Calculate the different fields from the address
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      unsigned set = (response.address >> caches[cacheLevel].offsetBits) & ((1 << caches[cacheLevel].setBits)-1);
      unsigned offset = ( response.address & ((1 << caches[cacheLevel].offsetBits)-1) ) >> 2;
      long line;
      // Timming: access
      response.time += caches[cacheLevel].access_time;
      // Statistics: access
      incrementIntegerStatistics(cacheName, "Accesses", 1);
      // Find tag in cache
      if((line = findTagInCache(operation->instructionOrData, cacheLevel, set, tag)) != -1) {
         // Hit
         printf(">   %s: Hit (%ld)\n", cacheName, line);
         // Statistics: hit
         incrementIntegerStatistics(cacheName, "Hits", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");
         // Prepare response 
         struct cacheLine cacheData;
         cacheData.content = malloc((sizeof(long))*caches[cacheLevel].numWords);
         // Read data from cache into response
         readLineFromCache(operation->instructionOrData, cacheLevel, &cacheData, line);
         if(response.size == 1) {
            response.data[0] = cacheData.content[offset];
            printf("Will get %d -> %d\n",offset, response.data[0]);
         } else {
         }
         // Remember cache level that resolved the request
         response.resolved = cacheLevel;
         // Stop iterating through caches
         break;
      } else {
         // Miss
         printf(">   %s: Miss 2^%d-1 = %f\n", cacheName,caches[cacheLevel].offsetBits, pow(2,caches[cacheLevel].offsetBits)-1);
         // Statistics: miss
         incrementIntegerStatistics(cacheName, "Misses", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");
         if(operation->operation == LOAD) {
            // Load operation
            // Upgrade request to a full cache line
            response.size = caches[cacheLevel].numWords;
            response.address &= -1 << caches[cacheLevel].offsetBits;
            free(response.data);
            response.data = malloc((sizeof(unsigned))*caches[cacheLevel].numWords);
         } else {
            // Write operation
            // Assuming WriteThrough and WriteNoAllocate do nothing
         }
      }
   }

   // Access memory if no cache level has resolved the request
   if(response.resolved < 0) {
      // Remember that the memory resolved the request
      response.resolved = numberCaches;
      // Statistics: memory
      incrementIntegerStatistics("Memory", "Accesses", response.size);
      if(operation->operation == LOAD) {
         // Load operation
         struct memoryPosition pos;
         // Timming: memory read
         response.time += memory.access_time_1;
         // Read data from memory into response
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=cpu.word_width/8) {
            if(readFromMemoryAddress(&pos, address) < 0) {
               fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
               return;
            }
            response.data[i] = pos.content;
         }
      } else { 
         // Store operation
         struct memoryPosition pos;
         pos.address = operation->address;
         pos.content = operation->data;
         // Timming: memory write
         response.time += memory.access_time_1;
         // Write data from request into memory
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=cpu.word_width/8) {
            pos.content = response.data[i];
            if(writeToMemoryAddress(&pos, address) < 0) {
               fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
               return;
            }
         }
      }
   }

   // Iterate backwards through all the cache levels that were involved in request 
   for(int cacheLevel = response.resolved-1; cacheLevel >= 0; cacheLevel--){
      sprintf(cacheName,"Cache L%d",cacheLevel+1);
      // Calculate the different fields from the address
      unsigned tag = response.address >> (caches[cacheLevel].offsetBits+caches[cacheLevel].setBits);
      unsigned set = (response.address >> caches[cacheLevel].offsetBits) & ((1 << caches[cacheLevel].setBits)-1);
      long line;
      // Find tag in cache
      if((line = findTagInCache(operation->instructionOrData, cacheLevel, tag, set)) > 0) {
         // Hit
         printf("<   %s: Hit\n", cacheName);
      } else {
         // Miss
         printf("<   %s: Miss\n", cacheName);
         if(operation->operation == LOAD) {
            // Load operation
            struct cacheLine cacheData;
            cacheData.dirty = 1;
            cacheData.valid = 1;
            cacheData.tag = tag;
            cacheData.content = response.data;
            int via = selectVia(operation->instructionOrData, cacheLevel, set);
            writeLineToCache(operation->instructionOrData, cacheLevel, &cacheData, via);
         } else {
            // Write operation
            // Assuming WriteThrough and WriteNoAllocate, do nothing!
         }
      }
   }
   printf("Got %d\n",response.data[0]);
   free(response.data);
   // Statistics: timming
   incrementDoubleStatistics("Totals", "Access Time", response.time);
   cycle++;
}

int selectVia(int instructionOrData, int cacheLevel, int set) {
   struct cacheLine cacheData;
   int lruLine = -1;
   int lruTime = -1;
   int lfuLine = -1;
   int lfuCount = -1;
   int fifoLine = -1;
   int fifoTime = -1;
   int firstLine = set*caches[cacheLevel].asociativity;
   for(int i = 0, via = rand() % caches[cacheLevel].asociativity; i < caches[cacheLevel].asociativity; i++, via=(via+1) % caches[cacheLevel].asociativity) {
      int line = firstLine + via;
      readFlagsFromCache(instructionOrData, cacheLevel, &cacheData, line);
      if(cacheData.valid == 0)
         return line;
      if(lruLine == -1 || lruTime > cacheData.lastAccess) {
         lruLine = line;
         lruTime = cacheData.lastAccess;
      }
      if(lfuLine == -1 || lfuCount > cacheData.accessCount) {
         lfuLine = line;
         lfuCount = cacheData.accessCount;
      }
      if(fifoLine == -1 || fifoTime > cacheData.firstAccess) {
         fifoLine = line;
         fifoTime = cacheData.firstAccess;
      }
   }
   // LRU=0, LFU=1, RANDOM=2, FIFO=3
   if(caches[cacheLevel].replacement == RANDOM) {
      return set * caches[cacheLevel].asociativity
             + rand() % caches[cacheLevel].asociativity;
   }
   else if(caches[cacheLevel].replacement == LRU) {
      return lruLine;
   }
   else if(caches[cacheLevel].replacement == LFU) {
      return lfuLine;
   }
   else if(caches[cacheLevel].replacement == FIFO) {
      return fifoLine;
   }
   else
      return set * caches[cacheLevel].asociativity;
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

