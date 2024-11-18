#include "mappingpolicy.h"
#include "simulator.h"


/**
 * @brief TODO
 * @param computer The computer that contains the caches and memory to be updated
 * @param cacheLevel The level of the cache that is getting updated
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void direct_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result){
      // The tag set and offset get calculated
      result->tag = operation->address >> (computer->cache[cacheLevel].offset_bits+computer->cache[cacheLevel].set_bits);
      result->set = (operation->address >> computer->cache[cacheLevel].offset_bits) & ((1 << computer->cache[cacheLevel].set_bits)-1);
      unsigned offset = ( operation->address & ((1 << computer->cache[cacheLevel].offset_bits)-1) ) >> 2;
}


/**
 * @brief Executes n way associativity and calculates the tag, set and offset of a certain memory operation.
 * @param computer The computer that contains the caches and memory to be updated
 * @param cacheLevel The level of the cache that is getting updated
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void set_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result){
      // The tag set and offset get calculated
      result->tag = operation->address >> (computer->cache[cacheLevel].offset_bits+computer->cache[cacheLevel].set_bits);
      result->set = (operation->address >> computer->cache[cacheLevel].offset_bits) & ((1 << computer->cache[cacheLevel].set_bits)-1);
      unsigned offset = ( operation->address & ((1 << computer->cache[cacheLevel].offset_bits)-1) ) >> 2;
}


/**
 * @brief TODO
 * @param computer The computer that contains the caches and memory to be updated
 * @param cacheLevel The level of the cache that is getting updated
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void fully_associative(Computer *computer, int cacheLevel, struct memOperation *operation, MappingResult *result){
      // The tag set and offset get calculated
      result->tag = operation->address >> (computer->cache[cacheLevel].offset_bits+computer->cache[cacheLevel].set_bits);
      result->set = (operation->address >> computer->cache[cacheLevel].offset_bits) & ((1 << computer->cache[cacheLevel].set_bits)-1);
      unsigned offset = ( operation->address & ((1 << computer->cache[cacheLevel].offset_bits)-1) ) >> 2;
}
