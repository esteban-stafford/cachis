#ifndef DATASTORE_H
#define DATASTORE_H

extern GtkTreeListModel *statistics_model;

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
	long startingAddress;
    gpointer user_data;
};

struct _CacheLineClass {
    GObjectClass parent_class;
};


// Nodes of the tree like structure of the statistics view
// The define, G_DECLARE and GObject parent_instance are required by GObject to store structs inside of GListStores
#define STATS_NODE_TYPE (stats_node_get_type())
G_DECLARE_FINAL_TYPE(StatsNode, stats_node, STATS, NODE, GObject)
struct _StatsNode {
	GObject parent_instance;	// Enables the struct to act like a node (Required by GObject)
	GListStore *children;	// Possible children of the node
	gchar *name;			// Name of the node
	gchar *content;			// Content of the node
	StatsNode *parent;		// Pointer to the parent node

	gboolean isComponent;	// If the stats node is a component or a property
	gboolean isExpanded;	// Handles collapsing of the GUI, by default all component tags are expanded.
	GtkWidget *container;	// Pointer to the GtkWidget that is contained by the element (Expander / Box)
};


enum {
  COMPONET_OR_PROPERTY = 0,
  VALUE=1,
  NUM_COLS=2
};

void generate_data_storage(Computer *computer);
void create_memory_model(Computer *computer);
void create_cache_model(Cache *cache, int level);
void create_statistics_model(Computer *computer);
void reset_memory_model(Computer *computer);
void reset_cache_model(Computer *computer);
void reset_statistics_model(Computer *computer);
void stats_node_set(StatsNode *node, gchar *name, gchar *content, StatsNode *parent, gboolean isComponent);

#endif
