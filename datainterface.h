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

void reset_memory(Computer *computer);
void reset_cache(Computer *computer, int level);

void show_line_from_cache(Computer *computer, int instructionOrData, int level, int i);
long find_tag_in_cache(Computer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag);
void read_line_from_cache(Computer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void read_flags_from_cache(Computer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber);
void write_line_to_cache(Computer *computer, int instructionOrData, int level, struct cacheLine *line, unsigned lineNumber);

int show_memory_address(Computer *computer, long address);
int read_from_memory_address(Computer *computer, struct memoryPosition *pos, long address);
int write_to_memory_address(Computer *computer, struct memoryPosition *pos, long address);

void set_statistics(char* component, char* property, char* value);
char* get_statistics(char* component, char* property);
void print_statistics(FILE* fp);

void remove_all_colors();

#endif
