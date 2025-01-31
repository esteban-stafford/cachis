/**
 * @file mappingpolicy.h
 * @brief Headers and constants related to mappingpolicy.c
 */

#ifndef MAPPINGPOLICY_H
#define MAPPINGPOLICY_H

#include "computer.h"
#include "simulator.h"

typedef struct {
    unsigned tag;
    unsigned set;
    unsigned offset;
} MappingResult;

void direct_associative(Cache *cache, MemoryOperation *operation, MappingResult *result);
void set_associative(Cache *cache, MemoryOperation *operation, MappingResult *result);
void fully_associative(Cache *cache, MemoryOperation *operation, MappingResult *result);

#endif
