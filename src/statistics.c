#include "statistics.h"
#include "gui.h"


/**
 * @brief Initiates the statistics
 * @param computer The computer.
 * @param operation The operation that was called
 * @param response The end result of the operation
 */
void init_statistics(Stats *stats) {
    // The statistics get initiated to 0
    stats->time = 0.0;
	stats->numBurstAccesses = 0;

    for (int i = 0; i < MAX_CACHES+1; i++) {
        stats->numAccesses[i] = 0;
        stats->numHits[i] = 0;
        stats->numMisses[i] = 0;
    }
}

/**
 * @brief Based on a response, the various statistics get calculated
 * @param computer The computer.
 * @param operation The operation that was called
 * @param response The end result of the operation
 */
void update_statistics(Computer *computer, Stats *stats) {
    char cacheName[20];

    // For every access, the global time, hit and miss statistics get updated
    for (int cacheLevel = 0; cacheLevel < computer->num_caches; cacheLevel++){
        sprintf(cacheName,"Cache L%d",cacheLevel+1);

        // The accesses
        stats->time += computer->cache[cacheLevel].access_time * stats->numAccesses[cacheLevel];
        increment_integer_statistics(cacheName, "Accesses", stats->numAccesses[cacheLevel]);

        // The hits
        increment_integer_statistics(cacheName, "Hits", stats->numHits[cacheLevel]);

        // The misses
        increment_integer_statistics(cacheName, "Misses", stats->numMisses[cacheLevel]);

        // The rates get calculated
        calculate_rate_statistics(cacheName, "Hit Rate", "Hits", "Accesses");
        calculate_rate_statistics(cacheName, "Miss Rate", "Misses", "Accesses");
    }

    // If the memory has been accessed, the memory access time get summed up to the total time as well (Just once)
    if (stats->numAccesses[MAX_CACHES] > 0) {
        increment_integer_statistics("Memory", "Accesses", stats->numAccesses[MAX_CACHES]);
        stats->time += computer->memory.access_time_1 * (stats->numAccesses[MAX_CACHES] - stats->numBurstAccesses);
		stats->time += computer->memory.access_time_burst * stats->numBurstAccesses;
    }

    increment_double_statistics("Totals", "Access Time", stats->time);
}

/**
 * This function is used to add a property or value to the simulation statistics panel
 * @param component String containing the name of the component
 * @param property String containing the name of the component's property
 * @param value String containing the value which that property will be set to.
 */
void set_statistics(char* component, char* property, char* value){
	// A pinter to the stats model and an interator are created
    GListModel *model = gtk_tree_list_model_get_model(statistics_model);
	guint num_children = g_list_model_get_n_items(model);
	guint num_properties;
	StatsNode *comp_node, *prop_node;

	// All the children of the root are iterated
	for (guint i = 0; i < num_children; i++) {
		comp_node = g_list_model_get_item(model, i);

		// If the children have the same name as component
		if (g_strcmp0(component, comp_node->name) == 0) {
			// Get the number of properties of that component
			num_properties = g_list_model_get_n_items(G_LIST_MODEL(comp_node->children));

			// Check if any of the properties match with the one that was provided as an argument
			for (guint j = 0; j < num_properties; j++) {
				prop_node = g_list_model_get_item(G_LIST_MODEL(comp_node->children), j);

				// If they match, update the value and exit.
				if (g_strcmp0(property, prop_node->name) == 0) {
					// The position of the previous value is stored
					guint position;
					g_list_store_find(comp_node->children, prop_node, &position);

					// The memory of the previous value is freed (If there was a value previously)
					if (g_strcmp0(prop_node->content, "") != 0) {
						free(prop_node->content);
					}

					// Memory for a value is allocated
					char *new_value = (char *)malloc(sizeof(char)*20);
					sprintf(new_value, "%s", value);

					// A new node is initiated
					StatsNode *new = g_object_new(STATS_NODE_TYPE, NULL);
					stats_node_set(new, property, new_value, comp_node, FALSE);

					// The state of the previous component is kept
					new->isExpanded = prop_node->isExpanded;

					// The previous node is removed
					g_list_store_remove(comp_node->children, position);

					// And it is inserted into the model
					g_list_store_insert(comp_node->children, position, new);
					g_object_ref(new);

					return;
				}
			}
			StatsNode *new = g_object_new(STATS_NODE_TYPE, NULL);
			stats_node_set(new, property, value, comp_node, FALSE);
			g_list_store_append(comp_node->children, new);

			return;
		}
	}
	// If there are no components with that name, create one and attach the property to that component
	StatsNode *new_c = g_object_new(STATS_NODE_TYPE, NULL);
	StatsNode *new_p = g_object_new(STATS_NODE_TYPE, NULL);
	stats_node_set(new_c, component, NULL, NULL, TRUE);
	stats_node_set(new_p, component, NULL, new_c, FALSE);
	g_list_store_append(new_c->children, new_p);
	g_list_store_append(G_LIST_STORE(model), new_c);
}



/**
 * This function is used to read a value from the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @return String containing th value
 */
char* get_statistics(char* component, char* property) {
	// A pinter to the stats model and an interator are created
    GListModel *model = gtk_tree_list_model_get_model(statistics_model);
	guint num_children = g_list_model_get_n_items(model);
	guint num_properties;
	StatsNode *comp_node, *prop_node;

	// All the children of the root are iterated
	for (guint i = 0; i < num_children; i++) {
		comp_node = g_list_model_get_item(model, i);

		// If the children have the same name as component
		if (g_strcmp0(component, comp_node->name) == 0) {
			// Get the number of properties of that component
			num_properties = g_list_model_get_n_items(G_LIST_MODEL(comp_node->children));

			// Check if any of the properties match with the one that was provided as an argument
			for (guint i = 0; i < num_properties; i++) {
				prop_node = g_list_model_get_item(G_LIST_MODEL(comp_node->children), i);

				// If they match, return the value.
				if (g_strcmp0(property, prop_node->name) == 0){
					return prop_node->content;
				}
			}
		}
	}

	// If the component and property have not been found, null is returned
	return NULL;
}



/**
 * This function is used to print simulation statistics panel
 * @param fp file to where it will be printed
 */
void print_statistics(FILE* fp) {
	fprintf(fp, "\n------SIMULATION STATISTICS------\n\n");
	// A pinter to the stats model and an interator are created
    GListModel *model = gtk_tree_list_model_get_model(statistics_model);
	guint num_children = g_list_model_get_n_items(model);
	guint num_properties;
	StatsNode *comp_node, *prop_node;

	// All the children of the root are iterated
	for (guint i = 0; i < num_children; i++) {
		// Print the component's name
		comp_node = g_list_model_get_item(model, i);
        printf("%s\n", comp_node->name);

		// Get the number of properties of the component
		num_properties = g_list_model_get_n_items(G_LIST_MODEL(comp_node->children));

		// Print the component names and contents / values
		for (guint i = 0; i < num_properties; i++) {
			prop_node = g_list_model_get_item(G_LIST_MODEL(comp_node->children), i);
            printf("          %s: %s\n", prop_node->name, prop_node->content);
		}
	}
}


void increment_double_statistics(char *component, char *property, double value) {
    double oldValue = 0.0;
    char *oldValueString = get_statistics(component,property);
    if(oldValueString)
        oldValue = strtod(oldValueString, NULL);
    char tmp[20];
    sprintf(tmp, "%lf", oldValue+value);
    set_statistics(component, property, tmp);
}

void increment_integer_statistics(char *component, char *property, int value) {
    int oldValue = 0.0;
    char *oldValueString = get_statistics(component,property);
    if(oldValueString)
        oldValue = atoi(oldValueString);
    char tmp[20];
    sprintf(tmp, "%d", oldValue+value);
    set_statistics(component, property, tmp);
}

void calculate_rate_statistics(char *component, char *property, char *partialName, char *totalName) {
    double partial = 0.0;
    double total = 0.0;
    char *valueString = get_statistics(component,partialName);
    if(valueString)
        partial = strtod(valueString, NULL);
    valueString = get_statistics(component,totalName);
    if(valueString)
        total = strtod(valueString, NULL);
    char tmp[20] = "NaN";
    if(total != 0) {
        sprintf(tmp, "%0.2lf", partial/total);
    }
    set_statistics(component, property, tmp);
}
