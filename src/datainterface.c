#include <math.h>
#include "datamanipulation.h"
#include "gui.h"
#include "datainterface.h"
#include "simulator.h"

//colors defined to be used in the program
const char* const colors[] = { "green", "red", "yellow", "green", "red", "grey", "orange", "purple", "pink", "lightblue", "lightgreen", "white"};

char *interfaceError = NULL;

#define RETURN_CACHIS_ERROR(code, message) interfaceError = message; return code;


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

    // Tag not found
    return -1;
}


/**
 * Reads the data from a cache line and updates the model and statistics. free_cache_data MUST be used after calling this function.
 * @param computer The computer.
 * @param instructionOrData If the cache is an instruction or data cache
 * @param level The level of the cache
 * @param line Pointer to a CacheLineContent struct to store the results
 * @param lineNumber Number of the cache line to fetch.
 */
void read_line_from_cache(Computer *computer, int instructionOrData, int level, CacheLineContent *line, int lineNumber) {
    GListModel *model;
    GtkWidget *view;

	// The model and the view (If running in GUI mode) are fetched
    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_data));
		}
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_instruction));
		}
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
    cache_line->last_accessed = cycle;

	// Since the stats get updated, the model needs to be modified as well
    gpointer items[] = { cache_line };
    g_list_store_splice(G_LIST_STORE(model), lineNumber, 1, items, 1);

    // Select and scroll to the updated row
	if (useGUI) {
		gtk_column_view_scroll_to(GTK_COLUMN_VIEW(view), lineNumber, NULL, GTK_LIST_SCROLL_SELECT ,NULL);
	}

    // Allocate memory for content and convert from string to array
    line->content = g_malloc(sizeof(long) * computer->cache[level].num_words);
    contentStringToArray(line->content, cache_line->content_cache, computer->cache[level].num_words);

    g_object_unref(item);
}


/**
 * Reads the data (including content) without updating the model. free_cache_data MUST be used after calling this function.
 * @param computer The computer.
 * @param instructionOrData If the cache is an instruction or data cache
 * @param level The level of the cache
 * @param line Pointer to a CacheLineContent struct to store the results
 * @param lineNumber Number of the cache line to fetch.
 */
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
	GtkWidget *view;

	// The model and the view (If running in GUI mode) are fetched
    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_data));
		}
    } else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_instruction));
		}
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
    // cache_line->color_cache = g_strdup(colors[WRITE]);
    cache_line->times_accessed = 1;
    cache_line->last_accessed = cycle;
    cache_line->first_accessed = cycle;
	cache_line->startingAddress = line->startingAddress;

    // Notify the model that the item has changed
    gpointer items[] = { cache_line };
    g_list_store_splice(G_LIST_STORE(model), lineNumber, 1, items, 1);

    // Select and scroll to the updated row
	if (useGUI) {
		gtk_column_view_scroll_to(GTK_COLUMN_VIEW(view), lineNumber, NULL, GTK_LIST_SCROLL_SELECT ,NULL);
	}

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
	GtkWidget *view;
    char contentString[1000];

    if (!computer->cache[level].separated || instructionOrData == DATA) {
        model = G_LIST_MODEL(computer->cache[level].model_data);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_data));
		}
	} else {
        model = G_LIST_MODEL(computer->cache[level].model_instruction);
		if (useGUI) {
			view = gtk_scrolled_window_get_child(GTK_SCROLLED_WINDOW(computer->cache[level].view_instruction));
		}
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
    g_list_store_remove(G_LIST_STORE(model), lineNumber);
    g_list_store_insert(G_LIST_STORE(model), lineNumber, cache_line);
    // gpointer items[] = { cache_line };
    // g_list_store_splice(G_LIST_STORE(model), lineNumber, 1, items, 1);

    // Select and scroll to the updated row
	if (useGUI) {
		gtk_column_view_scroll_to(GTK_COLUMN_VIEW(view), lineNumber, NULL, GTK_LIST_SCROLL_SELECT ,NULL);
	}

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

    // Only if the line has not been marked as written in the same cycle, is the background color updated
    // Change only if it has not been set to WRITE on the same cycle already
	if (!(g_strcmp0(memory_line->color, WRITE_COLOR) == 0 && memory_line->color_changed[ADDRESS] == cycle)) {
        memory_line->color = READ_COLOR;
        for (int i = 0; i < MEMORY_NUM_COLUMNS; i++) {
            memory_line->color_changed[i] = cycle;
        }
    }

    if (useGUI) {
        long int row = (pos->address - computer->memory.page_base_address) / 4;
        gtk_column_view_scroll_to(GTK_COLUMN_VIEW(view), row, NULL, GTK_LIST_SCROLL_SELECT ,NULL);
    }

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
    gpointer item = g_list_model_get_item(model, index);

    if (item == NULL) {
        RETURN_CACHIS_ERROR(-3, "Invalid memory address");
    }

    MemoryLine *memory_line = MEMORY_LINE(item);
    memory_line->address = pos->address;
    memory_line->content = pos->content;
    memory_line->color = WRITE_COLOR;

	for (int i = 0; i < MEMORY_NUM_COLUMNS; i++) {
		memory_line->color_changed[i] = cycle;
	}

    // Notify the model that the item has changed
    // g_list_store_remove(G_LIST_STORE(model), index);
    // g_list_store_insert(G_LIST_STORE(model), index, item);
    // gpointer items[] = { item };
    // g_list_store_splice(G_LIST_STORE(model), index, 1, items, 1);

    g_object_unref(item);
    return 0;
}


/**
 * Frees data allocated by read_flags_from_cache and read_line_from_cache.
 * @param line The line that contains the data
 */
void free_cache_data(CacheLineContent *line) {
    free(line->content);
}
