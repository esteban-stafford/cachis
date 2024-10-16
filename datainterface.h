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

void resetMemory(struct structComputer *computer);
void resetCache(struct structComputer *computer, int level);

void showLineFromCache(struct structComputer *computer, int instructionOrData, int level, int i);
long findTagInCache(struct structComputer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag);
void readLineFromCache(struct structComputer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void readFlagsFromCache(struct structComputer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void writeLineToCache(struct structComputer *computer, int instructionOrData, int level, struct cacheLine *line, unsigned lineNumber);

int showMemoryAddress(struct structComputer *computer, long address);
int readFromMemoryAddress(struct structComputer *computer, struct memoryPosition *pos, long address);
int writeToMemoryAddress(struct structComputer *computer, struct memoryPosition *pos, long address);

void setStatistics(char* component, char* property, char* value);
char* getStatistics(char* component, char* property);
void printStatistics(FILE* fp);

void removeAllColors();

#endif
