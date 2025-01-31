/**
 * @file replacementpolicy.h
 * @brief Headers and constants related to replacementpolicy.c
 */

#ifndef REPLACEMENTPOLICY_H
#define REPLACEMENTPOLICY_H

enum replacement {LRU=0, LFU=1, RANDOM=2, FIFO=3};

int select_line_to_replace(Computer *computer, int instructionOrData, int cacheLevel, int set);

#endif
