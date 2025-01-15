#include <ctype.h>

#include "datamanipulation.h"
#include "datastore.h"


// Private functions
GListModel *get_stats_node_children(GObject *item, gpointer user_data);

GtkTreeListModel *statistics_model;

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

// This is required
static void cache_line_class_init(CacheLineClass *class) { }


G_DEFINE_TYPE(StatsNode, stats_node, G_TYPE_OBJECT)

/**
 * @brief Creates a StatsNode for the tree view of the statistics.
 * @param node Pointer to the node
 * @param name Name of the node.
 * @param content Content of the node.
 */
static void stats_node_init(StatsNode *node) {
	// The node gets initiated no children gets created and returned
	node->name = NULL;
	node->content = NULL;
	node->children = g_list_store_new(STATS_NODE_TYPE);
	node->isComponent = FALSE;
	node->isExpanded = TRUE;
}


// This is required
static void stats_node_class_init(StatsNodeClass *class) { }

/**
 * @brief Creates a StatsNode for the tree view of the statistics.
 * @param node Pointer to the node
 * @param name Name of the node.
 * @param content Content of the node.
 */
void stats_node_set(StatsNode *node, gchar *name, gchar *content, StatsNode *parent, gboolean isComponent) {
	// The node gets initiated no children gets created and returned
	node->name = name;
	node->content = content;
	node->parent = parent;
	node->isComponent = isComponent;
}


/**
 * @brief Creates and populates the memory model
 * @param computer The computer that contains the memory and the model structure
 */
void createMemoryModel(Computer *computer) {
    // The model gets created
    GListStore *model = g_list_store_new(MEMORY_TYPE_LINE);
    int j = 0;

    // From the first to the last memory address in the trace
    for (unsigned long i = computer->memory.page_base_address;
         i < computer->memory.page_base_address + computer->memory.page_size; 
         i += (computer->cpu.word_width / 8), j++) {

        // A new memory line gets created
        MemoryLine *memory_line = g_object_new(MEMORY_TYPE_LINE,NULL);
        memory_line->address = i;
        memory_line->content = j;                   // Its value gets initiated to an increasing number

        // The line gets added to the model
        g_list_store_append(model, memory_line);
        g_object_unref(memory_line);
    }

    // A pointer to the model gets saved in the memory struct
    computer->memory.model = model;
    //g_object_unref(model);
}

/**
 * @brief Initiates a cache.
 * @param cache The cache to fill with cache lines
 * @param data_or_instruction if the cache contains data or instructions
 */
void create_cache_list_store(Cache *cache, int data_or_instruction) {
    GListStore *model = g_list_store_new(CACHE_LINE_TYPE);

    // The cache gets added num_lines lines.
    for (int i = 0; i < cache->num_lines; i++) {
        CacheLine *cache_line = g_object_new(CACHE_LINE_TYPE, NULL);

        //The basic fields get initiated
        cache_line->line = i;
        cache_line->set = (int) (i / cache->associativity);

        // The cache line is appended to the model
        g_list_store_append(model, cache_line);
        g_object_unref(cache_line);
    }

    // A pointer to the model is saved deppending on the type of the cache (D or I)
    if (data_or_instruction == 0) {
        cache->model_data = model;
    } else {
        cache->model_instruction = model;
    }
}

void createCacheModel(Cache *cache, int level) {
    create_cache_list_store(cache, 0);

    if (cache->separated) {
        create_cache_list_store(cache, 1);
    }

#if DEBUG
    fprintf(stderr, "cache level %d: %s lines: %d, associativity: %ld, sets: %d, words line: %d\n", 
            level + 1, cache->separated ? "separated" : "unified", cache->num_lines,
            cache->associativity, cache->num_sets, cache->num_words);
#endif
}

void generateDataStorage(Computer *computer){
   computer->cpu.buffer = NULL;
   createMemoryModel(computer);
   for(int i=0; i< computer->num_caches; i++){
      createCacheModel(&computer->cache[i], i);
      //reset_cache(i);
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
 * @brief Creates the tree structure of the statistics and stores the pointer in the statistics_model global variable.
 * @param computer The computer.
 */
void *create_model_statistics(Computer *computer) {
	// All the nodes get declared
	StatsNode *cpu, *mem, *caches[MAX_CACHES], *totals;
	StatsNode *cpu_accesses, *mem_accesses, *totals_atime;

	// A GListStore that will contain all the data gets created
    GListStore *root_store = g_list_store_new(STATS_NODE_TYPE);

	// CPU and Memory nodes get created, initiated and appended to the root node
	CacheLine *cache_line = g_object_new(CACHE_LINE_TYPE, NULL);
	cpu = g_object_new(STATS_NODE_TYPE, NULL);
	mem = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(cpu, S_CPU, NULL, NULL, TRUE);
	stats_node_set(mem, S_MEM, NULL, NULL, TRUE);
	g_list_store_append(root_store, cpu);
	g_list_store_append(root_store, mem);



	// For each level of cache, a new node gets appended
	for (int i = 0; i < computer->num_caches; i++) {
		caches[i] = g_object_new(STATS_NODE_TYPE, NULL);
		char *name = (char *)malloc(sizeof(char)*20);
		sprintf(name, "Cache L%d", i + 1);
		stats_node_set(caches[i], name, NULL, NULL, TRUE);
		g_list_store_append(root_store, caches[i]);

	}

	// A "Totals" section gets created
	totals = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(totals, S_TOTALS, NULL, NULL, TRUE);
	g_list_store_append(root_store, totals);


	// The properties of the CPU get assigned
	cpu_accesses = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(cpu_accesses, S_ACCESSES, "", cpu, FALSE);
	g_list_store_append(cpu->children, cpu_accesses);
	// g_object_unref(cpu_accesses);

	// The properties of the memory get assigned
	mem_accesses = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(mem_accesses, S_ACCESSES, "", mem, FALSE);
	g_list_store_append(mem->children, mem_accesses);
	// g_object_unref(mem_accesses);

	// The properties of the caches get assigned
	// StatsNode *c_accesses, *c_misses, *c_hits, *c_missrate, *c_hitrate;
	for (int i = 0; i < computer->num_caches; i++) {
		StatsNode *c_accesses, *c_misses, *c_hits, *c_missrate, *c_hitrate;
		c_accesses = g_object_new(STATS_NODE_TYPE, NULL);
		c_misses = g_object_new(STATS_NODE_TYPE, NULL);
		c_hits = g_object_new(STATS_NODE_TYPE, NULL);
		c_missrate = g_object_new(STATS_NODE_TYPE, NULL);
		c_hitrate = g_object_new(STATS_NODE_TYPE, NULL);

		stats_node_set(c_accesses, S_ACCESSES, "", caches[i], FALSE);
		stats_node_set(c_misses, S_MISSES, "", caches[i], FALSE);
		stats_node_set(c_hits, S_HITS, "", caches[i], FALSE);
		stats_node_set(c_missrate, S_MRATE, "", caches[i], FALSE);
		stats_node_set(c_hitrate, S_HRATE, "", caches[i], FALSE);

		g_list_store_append(caches[i]->children, c_accesses);
		g_list_store_append(caches[i]->children, c_misses);
		g_list_store_append(caches[i]->children, c_hits);
		g_list_store_append(caches[i]->children, c_missrate);
		g_list_store_append(caches[i]->children, c_hitrate);

		// g_object_unref(caches[i]);
	}
	// The properties of the totals get assigned
	totals_atime = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(totals_atime, S_ATIME, "", totals, FALSE);
	g_list_store_append(totals->children, totals_atime);
	// g_object_unref(totals_atime);

	// A GtkTreeListModel gets created
	statistics_model = gtk_tree_list_model_new(
		G_LIST_MODEL(root_store),
		TRUE,
		TRUE,
		(GtkTreeListModelCreateModelFunc) get_stats_node_children,
		NULL,
		NULL);

	// g_object_unref(cpu);
	// g_object_unref(mem);
	// g_object_unref(totals);

	return 0;
}



/**
 * @brief Gets the children of a StatsNode. Used for the GtkTreeListModel of the stats. The params are required by GTK.
 * @param item
 * @param user_data
 */
GListModel *get_stats_node_children(GObject *item, gpointer user_data) {
	// The chilren pointer is returned as a G_LIST_MODEL
	StatsNode *node = (StatsNode *)item;
	return G_LIST_MODEL(node->children);
}
