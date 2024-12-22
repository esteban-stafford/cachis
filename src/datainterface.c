#include <math.h>
#include "datamanipulation.h"
#include "gui.h"
#include "datainterface.h"
#include "simulator.h"

//colors defined to be used in the program
const char* const colors[] = { "green", "red", "yellow", "green", "red", "grey", "orange", "purple", "pink", "lightblue", "lightgreen", "white"};

char *interfaceError = NULL;

#define RETURN_CACHIS_ERROR(code, message) interfaceError = message; return code;

void scroll_to_row(GtkWidget *column_view, int percentage) {
    GtkAdjustment *vadjustment = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(column_view));
    guint max = gtk_adjustment_get_upper(vadjustment);
    guint page_size = gtk_adjustment_get_page_size(vadjustment);

    guint y = max * percentage / 100;

    //printf("Scrolling to %d%% (%d/%d)  --> ", percentage, y, max);
    if(y > max - page_size) {
        y = max - page_size;
    } else if(y < page_size/2) {
        y = 0;
    } else {
        y -= page_size / 2;
    }
    //printf("%d\n", y);
 
    gtk_adjustment_set_value(vadjustment, y);
}


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
    GListStore *model;

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
    }
}

/**
 * This function resets a Data cache. Sets the data cache to its initial state.
 * @param cache level where the cache is located. 
 */
void reset_cache(Computer *computer, int level){
   if(computer->cache[level].model_data)
      reset_cacheModel(computer, level, DATA);
   if(computer->cache[level].model_instruction) 
      reset_cacheModel(computer, level, INSTRUCTION);
}


/**
 * @brief For set associative cache and fully associative cache, find where the tag is located
 * @param computer The computer
 * @param instructionOrData If the line contains an instruction or data
 * @return -2 if error, -1 if miss or the address of the data
 */
long find_tag_in_cache(Computer *computer, int instructionOrData, int level, unsigned requestSet, unsigned requestTag) {
   //If the level is invalid, return -1
    if (level < 0 || level >= computer->num_caches) {
        return -2;
    }

    Cache *cache = &computer->cache[level];
    GListStore *model = (instructionOrData == 0) ? cache->model_instruction : cache->model_data;

    // Calculate the number of items in the set. If the cache is fully associative, this will return as many items as cache lines
    int items_in_set = cache->associativity;

    // Calculate the starting index for the requested set
    int start_index = requestSet * items_in_set;

    // Iterate through the lines in the set
    for (int i = 0; i < items_in_set; i++) {
        CacheLine *line = CACHE_LINE(g_list_model_get_item(G_LIST_MODEL(model), start_index + i));
        
        if (line) {
            if (line->valid && line->tag == requestTag) {
                // Tag found
                line->times_accessed++;
                line->last_accessed = cycle;
                return start_index + i;  // Return the index of the matching line
            }
        }
    }

    return -1;  // Tag not found
}


/**
 * This function reads a data cache line.
 * @param level which will be read
 * @param line. A pointer to a struct cacheLine were data will be placed. User must free line.content after calling the function. 
 * @param i line index
 */
void read_line_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, int lineNumber) {
    GListModel *model;
    GtkColumnView *view;

    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_data);
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_instruction);
    }

    gpointer item = g_list_model_get_item(model, lineNumber);
    if (item == NULL) {
        g_warning("Invalid cache line number: %d", lineNumber);
        return;
    }

    CacheLine *cache_line = CACHE_LINE(item);

    // Copy data from the model to the provided line struct
    line->line = cache_line->line;
    line->tag = cache_line->tag;
    line->set = cache_line->set;
    line->valid = cache_line->valid;
    line->dirty = cache_line->dirty;
    line->accessCount = cache_line->times_accessed;
    line->lastAccess = cache_line->last_accessed;
    line->firstAccess = cache_line->first_accessed;
	line->startingAddress = cache_line->startingAddress;
    
    // Update the model
    cache_line->color_cache = g_strdup(colors[READ]);
    cache_line->times_accessed++;
    cache_line->last_accessed = cycle;

	// Notify the model that the item has changed TODO This is broken, the model doesn't auto update anymore
    g_list_model_items_changed(model, lineNumber, 1, 1);

    // Scroll to the updated row
    //scroll_to_row(view, lineNumber * 100 / g_list_model_get_n_items(model));

    // Allocate memory for content and convert from string to array
    line->content = g_malloc(sizeof(long) * computer->cache[level].num_words);
    contentStringToArray(line->content, cache_line->content_cache, computer->cache[level].num_words);

    g_object_unref(item);
}

void read_flags_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, int lineNumber) {
    GListModel *model;

    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
    }

    gpointer item = g_list_model_get_item(model, lineNumber);
    if (item == NULL) {
        g_warning("Invalid cache line number: %d", lineNumber);
        return;
    }

    CacheLine *cache_line = CACHE_LINE(item);

    // Copy data from the model to the provided line struct
    line->line = cache_line->line;
    line->tag = cache_line->tag;
    line->set = cache_line->set;
    line->valid = cache_line->valid;
    line->dirty = cache_line->dirty;
    line->accessCount = cache_line->times_accessed;
    line->lastAccess = cache_line->last_accessed;
    line->firstAccess = cache_line->first_accessed;

	// Allocate memory for content and convert from string to array
    line->content = g_malloc(sizeof(long) * computer->cache[level].num_words);
    contentStringToArray(line->content, cache_line->content_cache, computer->cache[level].num_words);

    g_object_unref(item);

}


/**
 * This function updates the flags of a cache line.
 * @param level which will be written
 * @param line. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void write_flags_to_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, unsigned lineNumber) {
    GListModel *model;
    GtkColumnView *view;
    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_data);
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_instruction);
    }

    gpointer item = g_list_model_get_item(model, lineNumber);
    if (item == NULL) {
        g_warning("Invalid cache line number: %u", lineNumber);
        return;
    }

    CacheLine *cache_line = CACHE_LINE(item);
    cache_line->valid = line->valid;
    cache_line->dirty = line->dirty;
    cache_line->tag = line->tag;
    cache_line->color_cache = g_strdup(colors[WRITE]);
    cache_line->times_accessed = 1;
    cache_line->last_accessed = cycle;
    cache_line->first_accessed = cycle;
	cache_line->startingAddress = line->startingAddress;

    // Notify the model that the item has changed
    g_list_model_items_changed(model, lineNumber, 1, 1);

    // Scroll to the updated row
    // scroll_to_row(GTK_WIDGET(view), lineNumber * 100 / g_list_model_get_n_items(model));

    g_object_unref(item);
}

/**
 * This function writes a cache line.
 * @param level which will be written
 * @param line. A pointer to a struct cacheLine containing the data to be written.
 * @param i line index
 */
void write_line_to_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, unsigned lineNumber) {
    GListModel *model;
    GtkColumnView *view;
    char contentString[2000];
    
    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_data);
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
        // view = GTK_COLUMN_VIEW(computer->cache[level].view_instruction);
    }

    contentArrayToString(line->content, contentString, (computer->cache[level].line_size*8)/computer->cpu.word_width, computer->cpu.word_width/4);
    
    printf("\t Writing content in line %d of cache L%d: ", lineNumber, level + 1);
    for (int i = 0; i < computer->cache[level].num_words; i++) {
        printf("0x%x ", line->content[i]);
    }
    printf(" ---> %s\n", contentString);

    gpointer item = g_list_model_get_item(model, lineNumber);
    if (item == NULL) {
        g_warning("Invalid cache line number: %u", lineNumber);
        return;
    }

    CacheLine *cache_line = CACHE_LINE(item);
    cache_line->valid = line->valid;
    cache_line->dirty = line->dirty;
    cache_line->tag = line->tag;
    g_free(cache_line->content_cache);
    cache_line->content_cache = g_strdup(contentString);

    cache_line->color_cache = g_strdup(colors[WRITE]);
    cache_line->times_accessed = 1;
    cache_line->last_accessed = cycle;
    cache_line->first_accessed = cycle;
	cache_line->startingAddress = line->startingAddress;

    // Notify the model that the item has changed
    g_list_model_items_changed(model, lineNumber, 1, 1);

    // Scroll to the updated row
    // scroll_to_row(GTK_WIDGET(view), lineNumber * 100 / g_list_model_get_n_items(model));

    g_object_unref(item);
}


/**
 * This function shows a memory position
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int show_memory_address(Computer *computer, long address){
    MemoryPosition pos;
	// Read the memory position
   int returned = read_from_memory_address(computer, &pos, address);
   if (returned != 0){
      return returned;
   }
   // printf("Address: 0x%lx \t Content: 0x%lx\t User content: %s\n", pos.address, pos.content, (char*)pos.user_content);
   printf("Address: 0x%lx \t Content: 0x%lx\n", pos.address, pos.content);
   return 0;
}

/**
 * Prints the entire contents of the memory
 * @param computer The computer
 */
void print_memory_contents(Computer *computer) {
	printf("\n------MEMORY CONTENTS------\n\n");

	for (int i = computer->memory.page_base_address;
		i < computer->memory.page_base_address + computer->memory.page_size;
		i+=4) {
		show_memory_address(computer, i);
	}

}

void set_row_color(Computer *computer, int row_index, const char *color) {
    MemoryLine *line = g_list_model_get_item(G_LIST_MODEL(computer->memory.model), row_index);
    if (line) {
        line->color = color;
        //g_list_store_item_changed(computer->memory.model, row_index);
    }
}

/**
 * This function reads a memory position
 * @param pos. Read data will be placed in here. User must take care of freeing pos.user_content
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int read_from_memory_address(Computer *computer, MemoryPosition *pos, long address) {
    GListModel *model = G_LIST_MODEL(computer->memory.model);
    GtkColumnView *view = GTK_COLUMN_VIEW(computer->memory.view);

    // if not word address return error
    if (address % (computer->cpu.word_width / 8) != 0) {
        RETURN_CACHIS_ERROR(-1, "Not word address");
    }
    // if out of page return error
    if (address < computer->memory.page_base_address || 
        address > (computer->memory.page_base_address + computer->memory.page_size)) {
        RETURN_CACHIS_ERROR(-2, "Out of page");
    }

    // get the item from the memory address
    guint index = (address - computer->memory.page_base_address) / (computer->cpu.word_width / 8);
    guint max_index = g_list_model_get_n_items(model);
    gpointer item = g_list_model_get_item(model, index);

    if (item == NULL) {
        RETURN_CACHIS_ERROR(-3, "Invalid memory address");
    }

    MemoryLine *memory_line = MEMORY_LINE(item);
    pos->address = memory_line->address;
    pos->content = memory_line->content;

    set_row_color(computer, index, "#90a955");
    //scroll_to_row(computer->memory.view, index * 100 / max_index);

    g_object_unref(item);
    return 0;
}

/**
 * This function writes a memory position.
 * @param pos. Data to be written will be read from this struct.
 * @param address is the memory address
 * @return 0 if correct -1 if not word address error, -2 if out of page error
 */
int write_to_memory_address(Computer *computer, MemoryPosition *pos, long address) {
    GListModel *model = G_LIST_MODEL(computer->memory.model);
    GtkColumnView *view = GTK_COLUMN_VIEW(computer->memory.view);

    // if not word address return error
    if (address % (computer->cpu.word_width / 8) != 0) {
        RETURN_CACHIS_ERROR(-1, "Not word address");
    }
    // if out of page return error
    if (address < computer->memory.page_base_address ||
        address > (computer->memory.page_base_address + computer->memory.page_size)) {
        RETURN_CACHIS_ERROR(-2, "Out of page");
    }

    // get the item from the memory address
    guint index = (address - computer->memory.page_base_address) / (computer->cpu.word_width / 8);
    guint max_index = g_list_model_get_n_items(model);
    gpointer item = g_list_model_get_item(model, index);

    if (item == NULL) {
        RETURN_CACHIS_ERROR(-3, "Invalid memory address");
    }

    MemoryLine *memory_line = MEMORY_LINE(item);
    memory_line->address = pos->address;
    memory_line->content = pos->content;

    // Notify the model that the item has changed
    g_list_model_items_changed(model, index, 1, 1);

    set_row_color(computer, index, "#ff9b54");
    //scroll_to_row(computer->memory.view, index * 100 / max_index);

    g_object_unref(item);
    return 0;
}


/**
 * This function is used to remove all the colors from the cache and memory tables
 */
void remove_all_colors(Computer *computer){

   //remove colors from all caches
   // for(int i=0; i<computer->num_caches; i++){
   //     GtkTreeModel *model= GTK_TREE_MODEL(computer->cache[i].model_data);
   //     GtkTreeIter iter;
   //     int hasNext= gtk_tree_model_get_iter_first (model, &iter);
   //
   //     while(hasNext){
   //         gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
   //         hasNext=gtk_tree_model_iter_next (model, &iter);
   //     }
   //
   //     if(computer->cache[i].separated){
   //         model= GTK_TREE_MODEL(computer->cache[i].model_instruction);
   //         hasNext= gtk_tree_model_get_iter_first (model, &iter);
   //         while(hasNext){
   //            gtk_list_store_set (GTK_LIST_STORE(model), &iter, COLOR_CACHE, colors[WHITE], -1);
   //            hasNext=gtk_tree_model_iter_next (model, &iter);
   //         }
   //     }
   // }
   //
   // //remove colors from memory
   // GtkTreeIter iter;
   // int hasNext= gtk_tree_model_get_iter_first (GTK_TREE_MODEL(computer->memory.model), &iter);
   // while(hasNext){
   //    gtk_list_store_set (GTK_LIST_STORE(computer->memory.model), &iter, COLOR, colors[WHITE], -1);
   //    hasNext=gtk_tree_model_iter_next (GTK_TREE_MODEL(computer->memory.model), &iter);
   // }
}

