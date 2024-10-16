#include <ctype.h>

#include "datamanipulation.h"
#include "datastore.h"
//#include "gui.h"
//#include "datainterface.h"

/**
 * Funcntion that creates the data structures for memory data storage.
 *
 */
void createMemoryModel(Computer *computer){
   computer->memory.model = gtk_list_store_new(N_COLUMNS,
         G_TYPE_UINT,   /* ADDRESS */
         G_TYPE_UINT,    /* CONTENT */
         G_TYPE_STRING,   /* COLOR */
         G_TYPE_POINTER
         );
   for(unsigned long i=computer->memory.page_base_address; i<computer->memory.page_base_address+computer->memory.page_size; i+=(computer->cpu.word_width/8)){
      gtk_list_store_append(computer->memory.model, &iter);
      /* Fill fields with some data */
      gtk_list_store_set (computer->memory.model, &iter,
            ADDRESS, i,
            CONTENT, 0,
            //COLOR, "blue",
            -1);
   }
}

/**
 * Function that creates the data structures for a cache level.
 * @level whose data structure will be created.
 */
void createCacheModel(Cache *cache, int level){
   //Creo la cache. en caso de que sea dividia esta será la parte de data
   GtkListStore *model_data;
   GtkListStore *model_instruction;
   model_data= gtk_list_store_new(N_COLUMNS_CACHE,
         G_TYPE_UINT,     /* LINE=0, */
         G_TYPE_UINT,     /* SET=1, */
         G_TYPE_UINT,     /* VALID=2, */
         G_TYPE_UINT,     /* DIRTY=3, */
         G_TYPE_UINT,     /* TIMES_ACCESSED=4, */
         G_TYPE_UINT,     /* LAST_ACCESSED=5, */
         G_TYPE_UINT,     /* FIRST_ACCESSED=6, */
         G_TYPE_UINT,     /* TAG=7, */
         G_TYPE_STRING,   /* CONTENT_CACHE=8, */
         G_TYPE_STRING,   /* COLOR_CACHE=9, */
         G_TYPE_POINTER   /* USER_CONTENT_CACHE=10, */
         );               
   cache->model_data=model_data;
   for(int i=0; i<cache->num_lines; i++)
      gtk_list_store_append(model_data, &iter);

   if(cache->separated){
      model_instruction= gtk_list_store_new(N_COLUMNS_CACHE,
            G_TYPE_UINT,   /* LINE=0, */
            G_TYPE_UINT,   /* SET=1, */
            G_TYPE_UINT,   /* VALID=2, */
            G_TYPE_UINT,   /* DIRTY=3, */                    
            G_TYPE_UINT,   /* TIMES_ACCESSED=4, */
            G_TYPE_UINT,   /* LAST_ACCESSED=5, */
            G_TYPE_UINT,   /* FIRST_ACCESSED=6, */
            G_TYPE_UINT,   /* TAG=7, */
            G_TYPE_STRING, /* CONTENT_CACHE=8, */
            G_TYPE_STRING, /* COLOR_CACHE=9, */
            G_TYPE_POINTER /* USER_CONTENT_CACHE=10, */
            );             
      cache->model_instruction=model_instruction;
      for(int i=0; i<cache->num_lines; i++)
         gtk_list_store_append(model_instruction, &iter);
   }
#if DEBUG
         fprintf(stderr,"cache level %d: lines: %d, associativity: %ld, sets: %d, words line: %d\n", level+1, cache->num_lines, cache->associativity, cache->num_sets, cache->num_words);
#endif
      }

/**
 * Function that generates all data structures for the program.
 *
 */
void generateDataStorage(Computer *computer){
   createMemoryModel(computer);
   for(int i=0; i< computer->num_caches; i++){
      createCacheModel(&computer->cache[i], i);
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
