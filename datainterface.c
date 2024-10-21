#include <math.h>
#include "datamanipulation.h"
#include "gui.h"
#include "datainterface.h"
#include "simulator.h"

//colors defined to be used in the program
const char* const colors[] = { "green", "red", "yellow", "green", "red", "grey", "orange", "purple", "pink", "lightblue", "lightgreen", "white"};

char *interfaceError = NULL;

void reset_memory(Computer *computer) {
/*   GListStore *model = G_LIST_STORE(computer->memory.model);
   guint n_items = g_list_model_get_n_items(G_LIST_MODEL(model));
    
   for (guint i = 0; i < n_items; i++) {
      MemoryLine *memory_line = g_list_model_get_item(G_LIST_MODEL(model), i);
      g_object_set(memory_line, "content", 0, "color", "white", "user_data", NULL, NULL);
      g_object_unref(memory_line);
   } */
}

void reset_cacheModel(Computer *computer, int level, int instructionOrData) {
/*    GListStore *model;

    if (instructionOrData == DATA) {
        model = G_LIST_STORE(computer->cache[level].model_data);
    } else {
        model = G_LIST_STORE(computer->cache[level].model_instruction);
    }

    guint n_items = g_list_model_get_n_items(G_LIST_MODEL(model));
    
    // Prepare cache content
    unsigned cache_content[computer->cache[level].num_words];
    memset(cache_content, 0, sizeof(cache_content));
    
    // Convert cache content to string representation
    char cache_content_char[2000];
    contentArrayToString(cache_content, cache_content_char, 
                         (computer->cache[level].line_size * 8) / computer->cpu.word_width, 
                         computer->cpu.word_width / 4);

    for (guint i = 0; i < n_items; i++) {
        CacheLine *cache_line = g_list_model_get_item(G_LIST_MODEL(model), i);
        
        // Reset all fields of the cache line
        g_object_set(cache_line,
            "line", i,
            "tag", 0,
            "set", i / computer->cache[level].associativity,
            "content", cache_content_char,
            "user_content", NULL,
            "valid", 0,
            "dirty", 0,
            "times_accessed", 0,
            "last_accessed", 0,
            "first_accessed", 0,
            "color", "white",
            NULL);
        
        // Unref the cache line
        g_object_unref(cache_line);
    }*/
}

/**
 * This function resets a Data cache. Sets the data cache to its initial state.
 * @param cache level where the cache is located. 
 */
void reset_cache(Computer *computer, int level){
   /*if(computer->cache[level].model_data) 
      reset_cacheModel(computer, level, DATA);
   if(computer->cache[level].model_instruction) 
      reset_cacheModel(computer, level, INSTRUCTION);*/
}

/**
 * This function shows all the values contained in the fields of a data cache line
 * @param level which will be shown
 * @param i line index
 */
void show_line_from_cache(Computer *computer, int instructionOrData, int level, int i){
   /*struct cacheLine line;
   //first I read it
   read_line_from_cache(computer, instructionOrData, level, &line, i);
   char contentString[2000];
   contentArrayToString(line.content, contentString, (computer->cache[level].line_size*8)/computer->cpu.word_width, computer->cpu.word_width/4);
   //I print the values.
   printf("------------------------------------------------------\n");
   printf("line: %x     tag: %x    set: %x\n", line.line, line.tag, line.set);
   printf("content: %s\n", contentString);
   printf("user content: %s\n", (char*)line.user_content);
   printf("valid: %d   dirty: %d   last accessed: %d  times accessed: %d  first accessed: %d\n",
            line.valid, line.dirty, line.lastAccess, line.accessCount, line.firstAccess);
   printf("------------------------------------------------------\n");
   free(line.content); */
}

long find_tag_in_cache(Computer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag) {
   /* GtkTreeModel *model;
   GtkTreeIter iter;

   if(computer->cache[level].separated == 0 || instructionOrData == DATA) {
      model= GTK_TREE_MODEL(computer->cache[level].model_data);
      printf("Data\n");
   }
   else {
      model= GTK_TREE_MODEL(computer->cache[level].model_instruction);
      printf("Instruction %d %d %d==%d\n",level, computer->cache[level].separated, instructionOrData, DATA);
   }
   unsigned set,tag,valid,via=0;
   // Get first via in this set
   int more = gtk_tree_model_iter_nth_child (model, &iter, NULL, requestSet*computer->cache[level].associativity);

   while (more && via < computer->cache[level].associativity)
   {
      gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, SET, &set, TAG, &tag, VALID, &valid, -1);
      if(valid && tag == requestTag)
         return requestSet * computer->cache[level].associativity + via;
      // Get next via
      via++;
      more = gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter);
   } */
   return -1;
}

/**
 * This function reads a data cache line.
 * @param level which will be read
 * @param line. A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void read_line_from_cache(Computer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber){
   /*GtkTreeModel *model;
   GtkTreeView *view;
   GtkTreeIter iter;
   char *contentString;

   if(!computer->cache[level].separated || instructionOrData == DATA) {
      model = GTK_TREE_MODEL(computer->cache[level].model_data);
      view = GTK_TREE_VIEW(gui.cachePanel[level].viewData);
   }
   else {
      model= GTK_TREE_MODEL(computer->cache[level].model_instruction);
      view = GTK_TREE_VIEW(gui.cachePanel[level].viewInstruction);
   }
   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);

   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         CONTENT_CACHE, &contentString,
         USER_CONTENT_CACHE, &line->user_content,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->accessCount,
         LAST_ACCESSED, &line->lastAccess,
         FIRST_ACCESSED, &line->firstAccess,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR_CACHE, colors[READ],
         TIMES_ACCESSED, line->accessCount+1,
         LAST_ACCESSED, cycle,
         -1);
   char rowString[50];
   sprintf(rowString, "%d", lineNumber);
   gtk_tree_view_scroll_to_cell (view, gtk_tree_path_new_from_string(rowString), NULL, TRUE, 0.5, 0);
   //number of words in a cache line
   line->content = malloc((sizeof(long))*computer->cache[level].num_words);
   //turn String format to long array.
   contentStringToArray(line->content, contentString, computer->cache[level].num_words);
   g_free(contentString); */
}

void read_flags_from_cache(Computer *computer, int instructionOrData, int level, struct cacheLine* line, int lineNumber){
   /* GtkTreeModel *model;
   GtkTreeIter iter;
   char *contentString;

   if(!computer->cache[level].separated || instructionOrData == DATA)
      model= GTK_TREE_MODEL(computer->cache[level].model_data);
   else
      model= GTK_TREE_MODEL(computer->cache[level].model_instruction);

   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);

   gtk_tree_model_get (GTK_TREE_MODEL(model), &iter,
         LINE, &line->line,
         TAG, &line->tag,
         SET, &line->set,
         VALID, &line->valid,
         DIRTY, &line->dirty,
         TIMES_ACCESSED, &line->accessCount,
         LAST_ACCESSED, &line->lastAccess,
         FIRST_ACCESSED, &line->firstAccess,
         -1); */
}
/**
 * This function writes a cache line.
 * @param level which will be written
 * @param line. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void write_line_to_cache(Computer *computer, int instructionOrData, int level, struct cacheLine *line, unsigned lineNumber) {
   /* GtkTreeModel *model;
   GtkTreeView *view;
   GtkTreeIter iter;
   char contentString[2000];

   if(!computer->cache[level].separated || instructionOrData == DATA) {
      model = GTK_TREE_MODEL(computer->cache[level].model_data);
      view = GTK_TREE_VIEW(gui.cachePanel[level].viewData);
   }
   else {
      model= GTK_TREE_MODEL(computer->cache[level].model_instruction);
      view = GTK_TREE_VIEW(gui.cachePanel[level].viewInstruction);
   }
   contentArrayToString(line->content, contentString, (computer->cache[level].line_size*8)/computer->cpu.word_width, computer->cpu.word_width/4);
   printf("Write Content: ");
   for(int i = 0; i < computer->cache[level].num_words; i++) {
      printf("%x ",line->content[i]);
   }
   printf(" ---> %s\n", contentString);
   gtk_tree_model_iter_nth_child (model, &iter, NULL, lineNumber);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         VALID, line->valid,
         DIRTY, line->dirty,
         TAG, line->tag,
         CONTENT_CACHE, contentString, 
         COLOR_CACHE, colors[WRITE],
         TIMES_ACCESSED, 1,
         LAST_ACCESSED, cycle,
         FIRST_ACCESSED, cycle,
         -1);
   char rowString[50];
   sprintf(rowString, "%d", lineNumber);
   gtk_tree_view_scroll_to_cell (view, gtk_tree_path_new_from_string(rowString), NULL, TRUE, 0.5, 0); */
}

/**
 * This function shows a memory position
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int show_memory_address(Computer *computer, long address){
   /* struct memoryPosition pos;
   //I read the memory position
   int returned=read_from_memory_address(computer, &pos, address);
   if(returned!=0){
      return returned;
   }
   //I print the values
   printf("------------------------------------------------------\n");
   printf("address: %lx     content: %lx\n", pos.address, pos.content);
   printf("user content: %s\n", (char*)pos.user_content);
   printf("------------------------------------------------------\n"); */
   return 0;
}

/**
 * This function reads a memory position
 * @param pos. Read data will be placed in here. User must take care of freeing pos.user_content
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int read_from_memory_address(Computer *computer, struct memoryPosition *pos, long address){
   /* GtkTreeModel *model = GTK_TREE_MODEL(computer->memory.model);
   GtkTreeView *view = GTK_TREE_VIEW(gui.viewMEMORY);
   GtkTreeIter iter;
   // if not word address return error
   if(address % (computer->cpu.word_width/8)!=0){
      interfaceError = "not word address";
      return -1;
   }
   // if out of page return error
   if(address <computer->memory.page_base_address || address >(computer->memory.page_base_address+computer->memory.page_size)){
      interfaceError = "out of page";
      return -2;
   }
   // get the table row from the memory address
   gtk_tree_model_iter_nth_child (model,
         &iter,
         NULL,
         (address-computer->memory.page_base_address)/(computer->cpu.word_width/8));
   gtk_tree_model_get (model, &iter,
         ADDRESS, &pos->address,
         CONTENT, &pos->content,
         USER_CONTENT, &pos->user_content,
         -1);
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         COLOR, colors[READ],
         -1);
   char rowString[50];
   sprintf(rowString, "%ld", (address-computer->memory.page_base_address)/(computer->cpu.word_width/8));
   gtk_tree_view_scroll_to_cell (view, gtk_tree_path_new_from_string(rowString), NULL, TRUE, 0.5, 0); */
   return 0;
}
/**
 * This function writes a memory position.
 * @param pos. Data to be written will be read from this struct.
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int write_to_memory_address(Computer *computer, struct memoryPosition *pos, long address){
   /* GtkTreeModel *model= GTK_TREE_MODEL(computer->memory.model);
   GtkTreeView *view = GTK_TREE_VIEW(gui.viewMEMORY);
   GtkTreeIter iter;
   // if not word address return error
   if(address % (computer->cpu.word_width/8)!=0){
      interfaceError = "not word address";
      return -1;
   }
   //if out of page return error
   if(address < computer->memory.page_base_address || address > (computer->memory.page_base_address+computer->memory.page_size)){
      interfaceError = "out of page";
      return -2;
   }
   //get the table row from the memory address
   gtk_tree_model_iter_nth_child (model,
         &iter,
         NULL,
         (address-computer->memory.page_base_address)/(computer->cpu.word_width/8));
   gtk_list_store_set (GTK_LIST_STORE(model), &iter,
         ADDRESS, pos->address,
         CONTENT, pos->content,
         USER_CONTENT, pos->user_content,
         COLOR, colors[WRITE],
         -1);
   char rowString[50];
   sprintf(rowString, "%ld", (address-computer->memory.page_base_address)/(computer->cpu.word_width/8));
   gtk_tree_view_scroll_to_cell (view, gtk_tree_path_new_from_string(rowString), NULL, TRUE, 0.5, 0); */
   return 0;
}
/**
 * This function is used to add a property or value to the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @param value String containing the value which that property will be setted to.
 */
void set_statistics(char* component, char* property, char* value){
   /* GtkTreeModel *tree_model=statistics_model;
   GtkTreeIter iter;
   int isntEmpty=gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext&&isntEmpty){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //found
      if(!strcmp(component, name)){
         componentExists=1;
         break;
      }
      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet exists I search for the property
   if(componentExists){
      GtkTreeIter child;
      int hasChildren=gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      int hasNext=1;
      int propertyExists=0;
      //Search for the componets property
      while(hasNext&&hasChildren){
         char *name;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         //found
         if(!strcmp(property, name)){
            propertyExists=1;
            break;
         }
         hasNext=gtk_tree_model_iter_next (tree_model, &child);
      }
      //If the componet's property exists I set the value
      if(propertyExists){
         gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
               //COMPONET_OR_PROPERTY, "probando",
               VALUE, value,
               -1);
         //If the componet's property doesn't exist I add the property and set the value
      }else{
         gtk_tree_store_append(GTK_TREE_STORE(tree_model), &child, &iter);
         gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
               COMPONET_OR_PROPERTY, property,
               VALUE, value,
               -1);
      }
      //If the componet doesn't exist I create the componet and the property and I set the value
   }else{
      GtkTreeIter child;
      gtk_tree_store_append(GTK_TREE_STORE(tree_model), &iter, NULL);
      gtk_tree_store_set(GTK_TREE_STORE(tree_model), &iter,
            COMPONET_OR_PROPERTY, component,
            -1);
      gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      gtk_tree_store_append(GTK_TREE_STORE(tree_model), &child, &iter);
      gtk_tree_store_set(GTK_TREE_STORE(tree_model), &child,
            COMPONET_OR_PROPERTY, property,
            VALUE, value,
            -1);
   } */
}
/**
 * This function is used to read a value from the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @return String containing th value
 */
char* get_statistics(char* component, char* property){
   /* GtkTreeModel *tree_model=statistics_model;
   GtkTreeIter iter;
   gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //found
      if(!strcmp(component, name)){
         componentExists=1;
         break;
      }
      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet exists I search for the property
   if(componentExists){
      GtkTreeIter child;
      gtk_tree_model_iter_children (tree_model,
            &child,
            &iter);
      int hasNext=1;
      int propertyExists=0;
      //Search for the componet's property
      while(hasNext){
         char *name;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         //found
         if(!strcmp(property, name)){
            propertyExists=1;
            break;
         }
         hasNext=gtk_tree_model_iter_next (tree_model, &child);
      }
      //If the componet's property exists I get the value
      if(propertyExists){
         char* value;
         gtk_tree_model_get(GTK_TREE_MODEL(tree_model), &child,
               //COMPONET_OR_PROPERTY, "probando",
               VALUE, &value,
               -1);
         return value;
      }
   } */
   //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
   return NULL; 
}

/**
 * This function is used to print simulation statistics panel
 * @param fp file to where it will be printed
 */
void print_statistics(FILE* fp) {
   /* fprintf(fp, "\n------SIMULATION STATISTICS------\n\n");   
   GtkTreeModel *tree_model=statistics_model;
   GtkTreeIter iter;
   gtk_tree_model_get_iter_first (tree_model, &iter);
   int hasNext=1;
   int componentExists=0;
   //Search for the memory hierarchy componet
   while(hasNext){
      char *name;
      gtk_tree_model_get (tree_model, &iter,
            COMPONET_OR_PROPERTY, &name, -1);
      //print title
      printf("%s\n", name);
      //print children
      GtkTreeIter child;
      int hasNextProperty=gtk_tree_model_iter_children(tree_model,
            &child,
            &iter);

      while(hasNextProperty){
         char *name;
         char *value;
         gtk_tree_model_get (tree_model, &child,
               COMPONET_OR_PROPERTY, &name, -1);
         gtk_tree_model_get (tree_model, &child,
               VALUE, &value, -1);

         printf("        %s: %s\n", name, value);
         hasNextProperty=gtk_tree_model_iter_next (tree_model, &child);
      }



      hasNext=gtk_tree_model_iter_next (tree_model, &iter);
   }
   //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
   */
}


/**
 * This function is used to remove all the colors from the cache and memory tables
 */
void remove_all_colors(Computer *computer){
   /*
   //remove colors from all caches
   for(int i=0; i<computer->num_caches; i++){
       GtkTreeModel *model= GTK_TREE_MODEL(computer->cache[i].model_data);
       GtkTreeIter iter;
       int hasNext= gtk_tree_model_get_iter_first (model, &iter);

       while(hasNext){
           gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
           hasNext=gtk_tree_model_iter_next (model, &iter);
       }

       if(computer->cache[i].separated){
           model= GTK_TREE_MODEL(computer->cache[i].model_instruction);
           hasNext= gtk_tree_model_get_iter_first (model, &iter);
           while(hasNext){
              gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
              hasNext=gtk_tree_model_iter_next (model, &iter);
           }
       }
   }

   //remove colors from memory
   GtkTreeIter iter;
   int hasNext= gtk_tree_model_get_iter_first (GTK_TREE_MODEL(computer->memory.model), &iter);
   while(hasNext){
      gtk_list_store_set (GTK_LIST_STORE(computer->memory.model), &iter, COLOR, colors[WHITE], -1);
      hasNext=gtk_tree_model_iter_next (GTK_TREE_MODEL(computer->memory.model), &iter);
   } */
}

