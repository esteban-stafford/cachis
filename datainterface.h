#ifndef DATAINTERFACE_H
#define DATAINTERFACE_H

#include "gui.h"
#include "datastore.h"

enum {
    READ,
    WRITE,
    YELLOW,
    GREEN,
    RED,
    GREY,
    ORANGE,
    PURPLE,
    PINK,
    LIGHT_BLUE,
    LIGTH_GREE,
    WHITE,
};

struct cacheLine{
  unsigned line;
  unsigned tag;
  unsigned set;
  unsigned * content;

  void * user_content;

  unsigned valid;
  unsigned dirty;
  unsigned lastAccess;
  unsigned accessCount;
  unsigned firstAccess;
};

struct memoryPosition{
  long address;
  long content;
  void * user_content;
};

extern char *interfaceError;

void resetMemory();
void resetCache(int level);

void showLineFromCache(int instructionOrData, int level, int i);
long findTagInCache(int instructionOrData, int level, unsigned requestSet, unsigned requestTag);
void readLineFromCache(int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void readFlagsFromCache(int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void writeLineToCache(int instructionOrData, int level, struct cacheLine *line, unsigned lineNumber);

int showMemoryAddress(long address);
int readFromMemoryAddress(struct memoryPosition *pos, long address);
int writeToMemoryAddress(struct memoryPosition *pos, long address);

void setStatistics(char* component, char* property, char* value);
char* getStatistics(char* component, char* property);
void printStatistics(FILE* fp);

void removeAllColors();

#endif
