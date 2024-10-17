#ifndef DATASTORE_H
#define DATASTORE_H

extern GtkTreeModel *statistics_model;

#define MEMORY_TYPE_LINE (memory_line_get_type())
G_DECLARE_FINAL_TYPE(MemoryLine, memory_line, MEMORY, LINE, GObject)
struct _MemoryLine {
   GObject parent_instance;
    unsigned int address;
    unsigned int content;
    char *color;
    gpointer user_data;  // For user-defined data
};


#define CACHE_LINE_TYPE (cache_line_get_type())
G_DECLARE_FINAL_TYPE(CacheLine, cache_line, CACHE, LINE, GObject)
struct _CacheLine {
    unsigned int line;
    unsigned int set;
    unsigned int valid;
    unsigned int dirty;
    unsigned int times_accessed;
    unsigned int last_accessed;
    unsigned int first_accessed;
    unsigned int tag;
    char *content_cache;
    char *color_cache;
    gpointer user_data;  // Use gpointer for user-defined pointers
};


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

void createMemoryModel(Computer *computer);
void createCacheModel(Cache *cache, int level);
void writeBlankLine(int level, long line);
void insertTextInBuffer(char* text, GtkTextBuffer *buffer);
GtkTreeModel *create_model_statistics(Computer *computer);
void generateDataStorage(Computer *computer);

#endif
