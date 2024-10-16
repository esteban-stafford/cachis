#ifndef DATASTORE_H
#define DATASTORE_H

//#include "confparser.h"
// #include <gtk/gtk.h>

struct cacheLEVEL {
  GtkListStore *modelData;
  GtkListStore *modelInstruction;
};

GtkListStore *modelMEMORY; 

struct cacheLEVEL cacheLevels[MAX_CACHES];

GtkTreeModel *statistics_model;

enum {
    ADDRESS=0,
    CONTENT=1,
    COLOR=2,
    USER_CONTENT=3,
    N_COLUMNS=4
};

enum {
    LINE=0,
    SET=1,
    VALID=2,
    TAG=3,
    DIRTY=4,
    TIMES_ACCESSED=5,
    LAST_ACCESSED=6,
    FIRST_ACCESSED=7,
    CONTENT_CACHE=8,
    COLOR_CACHE=9,
    USER_CONTENT_CACHE=10,
    N_COLUMNS_CACHE=11
};

enum {
  COMPONET_OR_PROPERTY = 0,
  VALUE=1,
  NUM_COLS=2
};

void createMemoryModel(struct structComputer *computer);
void createCacheModel(struct structCache *cache, int level);
void writeBlankLine(int level, long line);
void insertTextInBuffer(char* text, GtkTextBuffer *buffer);
GtkTreeModel *create_model_statistics(struct structComputer *computer);
void generateDataStorage(struct structComputer *computer);

#endif
