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

typedef struct {
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
} CacheLineContent;

typedef struct {
  long address;
  long content;
  void * user_content;
} MemoryPosition;

extern char *interfaceError;



//Cache related functions
void reset_cache(Computer *computer, int level);
void show_line_from_cache(Computer *computer, int instructionOrData, int level, int i);
long find_tag_in_cache(Computer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag);
void read_line_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent* line, int lineNumber);
// void read_flags_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent* line, int lineNumber);
void write_line_to_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, unsigned lineNumber);

//Memory related functions
void reset_memory(Computer *computer);
int show_memory_address(Computer *computer, long address);
int read_from_memory_address(Computer *computer, MemoryPosition *pos, long address);
int write_to_memory_address(Computer *computer, MemoryPosition *pos, long address);

//Misc
void remove_all_colors();

#endif
