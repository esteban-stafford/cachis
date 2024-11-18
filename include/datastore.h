#ifndef DATASTORE_H
#define DATASTORE_H

extern GtkTreeModel *statistics_model;

#define MEMORY_TYPE_LINE (memory_line_get_type())
G_DECLARE_FINAL_TYPE(MemoryLine, memory_line, MEMORY, LINE, GObject)
struct _MemoryLine {
   GObject parent_instance;
    unsigned int address;
    unsigned int content;
    const char *color;
    gpointer user_data;
};

#define CACHE_LINE_TYPE (cache_line_get_type())
G_DECLARE_FINAL_TYPE(CacheLine, cache_line, CACHE, LINE, GObject)
struct _CacheLine {
   GObject parent_instance;
    unsigned int line;
    unsigned int set;
    unsigned int valid;
    unsigned int dirty;
    unsigned int times_accessed;
    unsigned int last_accessed;
    unsigned int first_accessed;
    unsigned int tag;
    char *content_cache;
    const char *color_cache; 
    gpointer user_data;
};

struct _CacheLineClass {
    GObjectClass parent_class;
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
