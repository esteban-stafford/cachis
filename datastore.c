#include <ctype.h>

#include "datamanipulation.h"
#include "datastore.h"

GtkTreeModel *statistics_model;

G_DEFINE_TYPE(MemoryLine, memory_line, G_TYPE_OBJECT)

static void memory_line_init(MemoryLine *memory_line) {
    memory_line->address = 0;
    memory_line->content = 0;
    memory_line->color = NULL;
    memory_line->user_data = NULL;
}

static void memory_line_class_init(MemoryLineClass *class) { }

G_DEFINE_TYPE(CacheLine, cache_line, G_TYPE_OBJECT)

static void cache_line_init(CacheLine *cache_line) {
    cache_line->line = 0;
    cache_line->set = 0;
    cache_line->valid = 0;
    cache_line->dirty = 0;
    cache_line->times_accessed = 0;
    cache_line->last_accessed = 0;
    cache_line->first_accessed = 0;
    cache_line->tag = 0;
    cache_line->content_cache = NULL;
    cache_line->color_cache = NULL;
    cache_line->user_data = NULL;
}

static void cache_line_class_init(CacheLineClass *class) { }

void createMemoryModel(Computer *computer) {
    GListStore *model = g_list_store_new(MEMORY_TYPE_LINE);
    for (unsigned long i = computer->memory.page_base_address; 
         i < computer->memory.page_base_address + computer->memory.page_size; 
         i += (computer->cpu.word_width / 8)) {
        MemoryLine *memory_line = g_object_new(MEMORY_TYPE_LINE,NULL);
        memory_line->address = i;
        g_list_store_append(model, memory_line);
        g_object_unref(memory_line);
    }
    computer->memory.model = model;
    g_object_unref(model);
}

static GListStore* create_cache_list_store(int num_lines) {
    GListStore *model = g_list_store_new(CACHE_LINE_TYPE);
    for (int i = 0; i < num_lines; i++) {
        CacheLine *cache_line = g_object_new(CACHE_LINE_TYPE, NULL);
        cache_line->line = i;
        g_list_store_append(model, cache_line);
        g_object_unref(cache_line);
    }
    return model;
}

void createCacheModel(Cache *cache, int level) {
    cache->model_data = create_cache_list_store(cache->num_lines);
    g_object_unref(cache->model_data);

    if (cache->separated) {
        cache->model_instruction = create_cache_list_store(cache->num_lines);
        g_object_unref(cache->model_instruction);
    }

#if DEBUG
    fprintf(stderr, "cache level %d: %s lines: %d, associativity: %ld, sets: %d, words line: %d\n", 
            level + 1, cache->separated ? "separated" : "unified", cache->num_lines,
            cache->associativity, cache->num_sets, cache->num_words);
#endif
}

void generateDataStorage(Computer *computer){
   createMemoryModel(computer);
   for(int i=0; i< computer->num_caches; i++){
      printf("Creating cache %d\n", i);
      createCacheModel(&computer->cache[i], i);
      printf("Cache %d created\n", i);
      //resetCache(i);
   }
   create_model_statistics(computer);
}

/**
 * Function that inserts text into the buffer.
 * @param text to be inserted.
 * @param buffer in which th etext will be inserted.
 */
void insertTextInBuffer(char* text, GtkTextBuffer *buffer){
   GtkTextMark *mark;
   GtkTextIter iter;
   mark = gtk_text_buffer_get_insert (buffer);
   gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
   gtk_text_buffer_insert (buffer, &iter, text, -1);
}

/**
 * Function that creates the data structure for simulation statistics.
 */
GtkTreeModel *create_model_statistics(Computer *computer){
   GtkTreeIter    toplevel, child;
   GtkTreeModel *model = GTK_TREE_MODEL(gtk_tree_store_new(NUM_COLS,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING));
   /* Append a top level row and leave it empty */
   gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
   gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
         COMPONET_OR_PROPERTY, "CPU",
         -1);
   /* Append a child to the top level row, and fill in some data */
   gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
   gtk_tree_store_set(GTK_TREE_STORE(model), &child,
         COMPONET_OR_PROPERTY, "Accesses",
         VALUE, "",
         -1);
   /* Append a second top level row, and fill it with some data */
   gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
   gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
         COMPONET_OR_PROPERTY, "Memory",
         -1);
   /* Append a child to the second top level row, and fill in some data */
   gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
   gtk_tree_store_set(GTK_TREE_STORE(model), &child,
         COMPONET_OR_PROPERTY, "Accesses",
         VALUE, "",
         -1);
   for(int i=0; i<computer->num_caches; i++){
      char currentCache[100];
      sprintf(currentCache, "Cache L%d", i+1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
      gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
            COMPONET_OR_PROPERTY, currentCache,
            -1);
      /* Append a child to the second top level row, and fill in some data */
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Accesses",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Misses",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Hits",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Miss Rate",
            VALUE, "",
            -1);
      gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
      gtk_tree_store_set(GTK_TREE_STORE(model), &child,
            COMPONET_OR_PROPERTY, "Hit Rate",
            VALUE, "",
            -1);
   }
   /* Append a last top level row, and fill it with some data */
   gtk_tree_store_append(GTK_TREE_STORE(model), &toplevel, NULL);
   gtk_tree_store_set(GTK_TREE_STORE(model), &toplevel,
         COMPONET_OR_PROPERTY, "Totals",
         -1);
   gtk_tree_store_append(GTK_TREE_STORE(model), &child, &toplevel);
   gtk_tree_store_set(GTK_TREE_STORE(model), &child,
         COMPONET_OR_PROPERTY, "Access Time",
         VALUE, "",
         -1);
   statistics_model= model;
   return GTK_TREE_MODEL(model);
}
