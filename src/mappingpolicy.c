/**
 * @file mappingpolicy.c
 * @brief Mapping policy related functions. Set, full and firect associative.
 */

#include "mappingpolicy.h"
#include "simulator.h"


/**
 * @brief Executes n way associativity and calculates the tag, set and offset of a certain memory operation.
 * @param cache The cache that is getting checked
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void set_associative(Cache *cache, MemoryOperation *operation, MappingResult *result) {
     // The tag set and offset get calculated
     unsigned shift;
     unsigned non_masked_address;

     /*
      * ---------------------------
      * |  tag  |   set  | offset |
      * ---------------------------
      */

     /* The tag gets calculated by shifting the address of the operation right by the number of offset bits (palabra) + set bits (conjunto)
        These values are precalculated in confparser using log_2 of the line size and log_2 of the number of sets (lines/associativity) respectively */
     shift = (cache->offset_bits + cache->set_bits);
     result->tag = operation->address >> shift;

     /* The set is calculated by shifting the address right offset_bits (removing the offset or palabra) and applying an AND with a mask.
      * The mask is calculated by shifting a 1, offset bits to the left, if there are 3 offset bits for instance, the 1 will become 0b1000. After
      * subtracting 1, the mask will become 0b111. After applying the mask the tag bits at the left of the set will be removed */
     non_masked_address = (operation->address >> cache->offset_bits);
     result->set = non_masked_address  & ((1 << cache->set_bits)-1);

     // The offset is calculated by applying an AND to the address so that the rightmost bits (the offset or palabra) are kept
     result->offset = (operation->address & ((1 << cache->offset_bits)-1)) >> 2;
}


/**
 * @brief Executes direct associativity and calculates the tag, set and offset of a certain memory operation.
 * @param cache The cache that is getting checked
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void direct_associative(Cache *cache, MemoryOperation *operation, MappingResult *result) {
     // It is calculated in the same way as set_associative, the only difference is that direct_associative uses sets of 1 instead of n
     // This only affects the way the cache replaces and accesses data, not how it calculates the fields.
     set_associative(cache, operation, result);
}


/**
 * @brief Executes full associativity and calculates the tag and offset of a certain memory operation. The set field is not updated.
 * @param computer The computer that contains the caches and memory to be updated
 * @param cacheLevel The level of the cache that is getting updated
 * @param operation The operation to perform
 * @param response Structure where the result will get stored
 */
void fully_associative(Cache *cache, MemoryOperation *operation, MappingResult *result) {
     // The tag and offset get calculated, since any line of the cache can be populated with any address, the set doesn't need to be calculated
     /*
      * ---------------------------
      * |     tag      | offset |
      * ---------------------------
      */

     // The tag gets calculated by removing the offset bits with a shift right
     result->tag = operation->address >> cache->offset_bits;

     // The offset is calculated in the same way as previously
     result->offset = ( operation->address & ((1 << cache->offset_bits)-1) ) >> 2;
}
