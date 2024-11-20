#include <stdio.h>
#include <string.h>
#include <math.h>
#include "confparser.h"
#include "datainterface.h"
#include "datamanipulation.h"
#include "simulator.h"
#include "mappingpolicy.h"

unsigned long cycle = 0;

/* Private functions */
void simFindInCache(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response);
void simOperateMemory(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response);
void simPopulateCache(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response);
void incrementDoubleStatistics(char *component, char *property, double value);
void incrementIntegerStatistics(char *component, char *property, int value);
void calculateRateStatistics(char *component, char *property, char *partial, char *total);
int selectVia(Computer *computer, int instructionOrData, int cacheLevel, int set);


/**
 * @brief Executes all the memory operations in one go
 */
void simulate(Computer *computer) {
   for(int i=0; i<numberOfOperations; i++){
      simulate_step(computer, &memoryOperations[i]);
   }
}


/**
 * @brief Simulates a single step
 * @param computer The computer to execute the operation
 * @param operation The operation to be executed
 */
void simulate_step(Computer *computer, struct memOperation *operation) {
   printf("---> Cycle %lu\n", cycle);
   // The response gets initiated
   struct response_type response;
   response.size = operation->size/4;
   response.address = operation->address;
   response.time = 0.0;
   response.data = malloc((sizeof(unsigned)));
   response.data[0] = operation->data;
   response.resolved = -1;
   char cacheName[20];

   printf("Simulating operation: ");
   printMemOperation(stdout, operation, computer->cpu.address_width);
   incrementIntegerStatistics("CPU", "Accesses", 1);
 
   // Try to find the value in the caches
   simFindInCache(computer, operation, cacheName, &response);

   // If no cache level resolved the request, the memory gets accessed
   if (response.resolved < 0) {
      simOperateMemory(computer, operation, cacheName, &response);
   }

   // Lastly, since the data has been requested by the CPU, all levels above response.resolved have to been populated
   // until it reaches the top level the CPU has access to.
   simPopulateCache(computer, operation, cacheName, &response);

   // The action gets printed
   printf("Got %d\n",response.data[0]);      // The data that was operated with
   free(response.data);
   incrementDoubleStatistics("Totals", "Access Time", response.time);
   cycle++;

   // And all the statistics also get printed as well
   print_statistics(stdout);


   printf("\n----------------\n\n");
}

/**
 * @brief Iterates through all caches to find if a value is in cache and updates response.
 * @param computer The computer.
 * @param memOperation The operation to perform on the caches.
 * @param charName Printinf information.
 * @param response Statistics about the access. Will get updated with time and the level of cache that contains the data.
 */
void simFindInCache(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response) {
   // Iterate through all the cache levels
   for(int cacheLevel = 0; cacheLevel < computer->num_caches; cacheLevel++){
      MappingResult mappingResult;
      long line;

      sprintf(cacheName,"Cache L%d",cacheLevel+1);

      // The associativity of the cache gets checked and the corresponding function gets called
      if (computer->cache[cacheLevel].associativity == 1) {        // Direct Mapping
         direct_associative(computer, cacheLevel, operation, &mappingResult);
      } else if (computer->cache[cacheLevel].associativity > 1) {  // N way set associative
         set_associative(computer, cacheLevel, operation, &mappingResult);
      } else {                                                     //Fully associative
         fully_associative(computer, cacheLevel, operation, &mappingResult);
      }

      // The timing is updated according to the cache's characteristics
      response->time += computer->cache[cacheLevel].access_time;

      // The number of accesses gets incremented by one
      incrementIntegerStatistics(cacheName, "Accesses", 1);

      // Find tag in cache
      line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, mappingResult.set, mappingResult.tag);

      // If there was an error, print it and return without making changes
      if(line == -2){
         printf("Error: The level of the cache was lower than 0 or exceeded the maximum.\n");
         return;
      }

      // If there is a hit and the address has been found
      if (line > -1) {
         // Hit
         printf(">   %s: Hit (%ld)\n", cacheName, line);

         // The statistics get updated
         incrementIntegerStatistics(cacheName, "Hits", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");

         // A response gets prepared
         struct cacheLine cacheData;
         cacheData.content = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);

         // Read data from cache into response
         read_line_from_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);
         if (response->size == 1) {
            response->data[0] = cacheData.content[mappingResult.offset];
            printf("Will get %d -> %d\n",mappingResult.offset, response->data[0]);
         } else {
         }

         // Remember cache level that resolved the request
         response->resolved = cacheLevel;

         // Since there has been a hit, there's no need to reach the lower levels of the cache, the loop ends
         break;
      } else {    //If there is a miss
         // Miss
         printf(">   %s: Miss 2^%d-1 = %f\n", cacheName,computer->cache[cacheLevel].offset_bits, pow(2,computer->cache[cacheLevel].offset_bits)-1);

         // The statistics get updated
         incrementIntegerStatistics(cacheName, "Misses", 1);
         calculateRateStatistics(cacheName, "Hit Rate", "Hits", "Accesses");
         calculateRateStatistics(cacheName, "Miss Rate", "Misses", "Accesses");

         // If the operation is a LOAD
         if (operation->operation == LOAD) {
            // Upgrade request to a full cache line
            response->size = computer->cache[cacheLevel].num_words;
            response->address &= -1 << computer->cache[cacheLevel].offset_bits;
            free(response->data);
            response->data = malloc((sizeof(unsigned))*computer->cache[cacheLevel].num_words);
         } else {    // If the operation is a STORE
            //TODO Implement write operations (WriteThrough,Writeback)
            // Write operation
            // Assuming WriteThrough and WriteNoAllocate do nothing
         }
      }
   }

}


/**
 * @brief Accesses memory to load or store data.
 * @param computer The computer.
 * @param memOperation The operation to perform on the memory.
 * @param charName Printinf information.
 * @param response Statistics about the access.
 */
void simOperateMemory(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response) {
   // Remember that the memory resolved the request
   response->resolved = computer->num_caches;    // The number of caches is used to signify that it has been through all of them

   // Statistics get updated
   incrementIntegerStatistics("Memory", "Accesses", response->size);

   // If the operation is a LOAD
   if(operation->operation == LOAD) {
      struct memoryPosition pos;

      // The time to access the main memory is noted
      response->time += computer->memory.access_time_1;

      // And the data is read from memory
      // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
      // After every iteration, the address increases one word
      for(unsigned i=0, address=response->address; i < response->size; i++, address+=computer->cpu.word_width/8) {
         if(read_from_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
         }
         response->data[i] = pos.content;
      }
   } else {       // If the operation is a STORE      // TODO. Check if a write policy needs to be implemented into this
      struct memoryPosition pos;

      //The address and content gets noted
      pos.address = operation->address;
      pos.content = operation->data;

      // The time to write to main memory gets noted
      response->time += computer->memory.access_time_1;

      // Write data from request into memory
      // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
      // After every iteration, the address increases one word
      for(unsigned i=0, address=response->address; i < response->size; i++, address+=computer->cpu.word_width/8) {
         pos.content = response->data[i];
         if(write_to_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
         }
      }
   }
}

/**
 * @brief Populates all caches above the line that resolved the request
 * @param computer The computer.
 * @param memOperation The operation to perform on the caches.
 * @param charName Printinf information.
 * @param response Statistics about the access.
 */
void simPopulateCache(Computer *computer, struct memOperation *operation, char *cacheName, struct response_type *response) {
   // Iterate backwards through all the cache levels that were involved in request
   for (int cacheLevel = response->resolved-1; cacheLevel >= 0; cacheLevel--){
      MappingResult mappingResult;
      long line;

      sprintf(cacheName,"Cache L%d",cacheLevel+1);
      // Calculate the different fields from the address
      // The associativity of the cache gets checked and the corresponding function gets called
      if (computer->cache[cacheLevel].associativity == 1) {        // Direct Mapping
         direct_associative(computer, cacheLevel, operation, &mappingResult);
      } else if (computer->cache[cacheLevel].associativity > 1) {  // N way set associative
         set_associative(computer, cacheLevel, operation, &mappingResult);
      } else {                                                     //Fully associative
         fully_associative(computer, cacheLevel, operation, &mappingResult);
      }

      // Find tag in cache
      line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, mappingResult.tag, mappingResult.set);

      // If there was an error, print it and return without making changes
      if(line == -2){
         printf("Error: The level of the cache was lower than 0 or exceeded the maximum.\n");
         return;
      }

      // If the line was found in cache, there's a hit
      if(line > 0) {
         // Hit
         printf("<   %s: Hit\n", cacheName);
      } else {       //If not, miss
         // Miss
         printf("<   %s: Miss\n", cacheName);
         if(operation->operation == LOAD) {
            // Load operation
            struct cacheLine cacheData;
            cacheData.dirty = 1;
            cacheData.valid = 1;
            cacheData.tag = mappingResult.tag;
            cacheData.content = response->data;

            // The via gets located and populated with the data
            int via = selectVia(computer, operation->instructionOrData, cacheLevel, mappingResult.set);
            write_line_to_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, via);
         } else {
            //TODO Implement Write Operations
            // Write operation
            // Assuming WriteThrough and WriteNoAllocate, do nothing!
         }
      }
   }

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
