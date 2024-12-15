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
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @param value String containing the value which that property will be setted to.
 */
void set_statistics(char* component, char* property, char* value){
    GtkTreeModel *tree_model=statistics_model;
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
    }
}
/**
 * This function is used to read a value from the simulation statistics panel
 * @param component String containig the name of the componet
 * @param property String containig the name of the component's property
 * @return String containing th value
 */
char* get_statistics(char* component, char* property){
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
    }
    //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
    return NULL;
}

/**
 * This function is used to print simulation statistics panel
 * @param fp file to where it will be printed
 */
void print_statistics(FILE* fp) {
     fprintf(fp, "\n------SIMULATION STATISTICS------\n\n");
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

            printf("          %s: %s\n", name, value);
            hasNextProperty=gtk_tree_model_iter_next (tree_model, &child);
        }

        hasNext=gtk_tree_model_iter_next (tree_model, &iter);
    }
    //If the componet or the property don't exist I create the componet and the property and I set the value return param to NULL
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
