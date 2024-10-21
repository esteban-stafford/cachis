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
int selectVia(Computer *computer, int instructionOrData, int cacheLevel, int set);

struct response_type {
   double time;
   int resolved;
   unsigned address;
   unsigned size;
   unsigned *data;
};

void simulate(Computer *computer) {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(computer, &memoryOperations[i]);
   }
}

void simulate_step(Computer *computer, struct memOperation *operation) {
   struct response_type response;
   char cacheName[20];
   response.size = operation->size/4;
   response.address = operation->address;
   response.time = 0.0;
   response.data = malloc((sizeof(unsigned)));
   response.data[0] = operation->data;
   response.resolved = -1;

   printf("Simulating operation: ");
   printMemOperation(stdout, operation, computer->cpu.address_width);
   incrementIntegerStatistics("CPU", "Accesses", 1);
 
   // Iterate through all the cache levels
   for(int cacheLevel = 0; cacheLevel < computer->num_caches; cacheLevel++){
      sprintf(cacheName,"Cache L%d",cacheLevel+1);
      // Calculate the different fields from the address
      unsigned tag = response.address >> (computer->cache[cacheLevel].offset_bits+computer->cache[cacheLevel].set_bits);
      unsigned set = (response.address >> computer->cache[cacheLevel].offset_bits) & ((1 << computer->cache[cacheLevel].set_bits)-1);
      unsigned offset = ( response.address & ((1 << computer->cache[cacheLevel].offset_bits)-1) ) >> 2;
      long line;
      // Timming: access
      response.time += computer->cache[cacheLevel].access_time;
      // Statistics: access
      incrementIntegerStatistics(cacheName, "Accesses", 1);
      // Find tag in cache
      if((line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, set, tag)) != -1) {
         // Hit
         printf(">   %s: Hit (%ld)\n", cacheName, line);
         // Statistics: hit
         incrementIntegerStatistics(cacheName, "Hits", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");
         // Prepare response 
         struct cacheLine cacheData;
         cacheData.content = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);
         // Read data from cache into response
         read_line_from_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);
         if(response.size == 1) {
            response.data[0] = cacheData.content[offset];
            printf("Will get %d -> %d\n",offset, response.data[0]);
         } else {
         }
         // Remember cache level that resolved the request
         response.resolved = cacheLevel;
         // Stop iterating through computer->cache
         break;
      } else {
         // Miss
         printf(">   %s: Miss 2^%d-1 = %f\n", cacheName,computer->cache[cacheLevel].offset_bits, pow(2,computer->cache[cacheLevel].offset_bits)-1);
         // Statistics: miss
         incrementIntegerStatistics(cacheName, "Misses", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");
         if(operation->operation == LOAD) {
            // Load operation
            // Upgrade request to a full cache line
            response.size = computer->cache[cacheLevel].num_words;
            response.address &= -1 << computer->cache[cacheLevel].offset_bits;
            free(response.data);
            response.data = malloc((sizeof(unsigned))*computer->cache[cacheLevel].num_words);
         } else {
            // Write operation
            // Assuming WriteThrough and WriteNoAllocate do nothing
         }
      }
   }

   // Access memory if no cache level has resolved the request
   if(response.resolved < 0) {
      // Remember that the memory resolved the request
      response.resolved = computer->num_caches;
      // Statistics: memory
      incrementIntegerStatistics("Memory", "Accesses", response.size);
      if(operation->operation == LOAD) {
         // Load operation
         struct memoryPosition pos;
         // Timming: memory read
         response.time += computer->memory.access_time_1;
         // Read data from memory into response
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=computer->cpu.word_width/8) {
            if(read_from_memory_address(computer, &pos, address) < 0) {
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
         response.time += computer->memory.access_time_1;
         // Write data from request into memory
         for(unsigned i=0, address=response.address; i < response.size; i++, address+=computer->cpu.word_width/8) {
            pos.content = response.data[i];
            if(write_to_memory_address(computer, &pos, address) < 0) {
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
      unsigned tag = response.address >> (computer->cache[cacheLevel].offset_bits+computer->cache[cacheLevel].set_bits);
      unsigned set = (response.address >> computer->cache[cacheLevel].offset_bits) & ((1 << computer->cache[cacheLevel].set_bits)-1);
      long line;
      // Find tag in cache
      if((line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, tag, set)) > 0) {
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
            int via = selectVia(computer, operation->instructionOrData, cacheLevel, set);
            write_line_to_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, via);
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

int selectVia(Computer *computer, int instructionOrData, int cacheLevel, int set) {
   struct cacheLine cacheData;
   Cache *cache = &computer->cache[cacheLevel];
   int lruLine = -1;
   int lruTime = -1;
   int lfuLine = -1;
   int lfuCount = -1;
   int fifoLine = -1;
   int fifoTime = -1;
   int firstLine = set*cache->associativity;
   for(int i = 0, via = rand() % cache->associativity; i < cache->associativity; i++, via=(via+1) % cache->associativity) {
      int line = firstLine + via;
      read_flags_from_cache(computer, instructionOrData, cacheLevel, &cacheData, line);
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
   if(cache->replacement_policy == RANDOM) {
      return set * cache->associativity
             + rand() % cache->associativity;
   }
   else if(cache->replacement_policy == LRU) {
      return lruLine;
   }
   else if(cache->replacement_policy == LFU) {
      return lfuLine;
   }
   else if(cache->replacement_policy == FIFO) {
      return fifoLine;
   }
   else
      return set * cache->associativity;
}

void incrementDoubleStatistics(char *component, char *property, double value) {
   double oldValue = 0.0;
   char *oldValueString = get_statistics(component,property);
   if(oldValueString) 
      oldValue = strtod(oldValueString, NULL);
   char tmp[20];
   sprintf(tmp, "%lf", oldValue+value);
   set_statistics(component, property, tmp);
}

void incrementIntegerStatistics(char *component, char *property, int value) {
   int oldValue = 0.0;
   char *oldValueString = get_statistics(component,property);
   if(oldValueString) 
      oldValue = atoi(oldValueString);
   char tmp[20];
   sprintf(tmp, "%d", oldValue+value);
   set_statistics(component, property, tmp);
}

void calculateRateStatistics(char *component, char *property, char *partialName, char *totalName) {
   double partial = 0.0;
   double total = 0.0;
   char *valueString = get_statistics(component,partialName);
   if(valueString) 
      partial = strtod(valueString, NULL);
   valueString = get_statistics(component,totalName);
   if(valueString) 
      total = strtod(valueString, NULL);
   char tmp[20] = "NaN";
   if(total != 0) {
      sprintf(tmp, "%0.2lf", partial/total);
   }
   set_statistics(component, property, tmp);
}

