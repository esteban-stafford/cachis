/**
 * @file datainterface.h
 * @brief Headers and constants related to datainterface.c
 */

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
  long startingAddress;
} CacheLineContent;

typedef struct {
  long address;
  long content;
  void * user_content;
} MemoryPosition;

extern char *interfaceError;
extern int generate_dramsys_trace;
extern char *dramsys_file;

//CacheLine related functions
unsigned read_word_from_cache_line(Computer *computer, CacheLineContent *line, int wordNumber);
void write_word_from_cache_line(Computer *computer, CacheLineContent *line, int wordNumber);

//Cache related functions
void show_line_from_cache(Computer *computer, int instructionOrData, int level, int i);
long find_tag_in_cache(Computer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag);
void read_line_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent* line, int lineNumber);
void read_flags_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent* line, int lineNumber);
void write_line_to_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, unsigned lineNumber);
void write_flags_to_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, unsigned lineNumber);

//Memory related functions
void print_memory_contents(Computer *computer);
int show_memory_address(Computer *computer, long address);
int read_from_memory_address(Computer *computer, MemoryPosition *pos, long address);
int write_to_memory_address(Computer *computer, MemoryPosition *pos, long address);
void free_cache_data(CacheLineContent *line);

#endif
