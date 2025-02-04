/**
 * @file writepolicy.h
 * @brief Headers and constants related to writepolicy.c
 */

#ifndef WRITEPOLICY_H
#define WRITEPOLICY_H

#include "computer.h"
#include "datainterface.h"
#include "traceparser.h"
#include "simulator.h"
#include "statistics.h"

enum write_policy {WRITE_THROUGH=0, WRITE_BACK=1};

void write_back(Computer *computer, MemoryOperation *operation, ResponseType *response, int cacheLevel, int cacheLine);
void write_through(Computer *computer, MemoryOperation *operation, ResponseType *response);
#endif
