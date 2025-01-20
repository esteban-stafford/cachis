#include <ctype.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "datastore.h"
#include "traceparser.h"
#include "simulator.h"


static GtkTextTag *highlight_tag = NULL;
static GtkTextMark *previous_highlight_mark = NULL;


// Private functions
int launch_gui(int argc, char **argv, Computer *computer);
static void activate(GtkApplication *app, gpointer user_data);

// Setups
// static GMenu *create_menubar();
static GtkWidget *create_left_column(Computer *computer);
static GtkWidget *create_middle_section(Computer *computer);
static GtkWidget *create_right_column(Computer *computer);
static GtkWidget *create_toolbar(Computer *computer);
static GtkWidget *create_cache_widget(Cache *cache);
static GtkWidget *create_cache_table(GListStore *model);
static GtkWidget *create_memory_table(Computer *computer);

// Binds
static void setup_cb(GtkSignalListItemFactory *factory, GObject *listitem);
static void setup_cache_cb(GtkSignalListItemFactory *factory, GObject *listitem);
static void bind_address_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_line_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_set_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_valid_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_dirty_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_accessed_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_last_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_first_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_tag_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_cache_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem);
static void bind_stats_row(GtkListItemFactory *factory, GtkListItem *list_item);

// Callbacks
static void on_expander_toggled(GtkExpander *expander, GParamSpec *pspec, StatsNode *data);
static void on_run_to_breakpoint_clicked(GtkButton *button, Computer *computer);
static void on_step_button_clicked(GtkButton *button, Computer *computer);
static void on_reset_button_clicked(GtkButton *button, Computer *computer);

// Misc
int step_trace_line(char *line, Computer *computer);
int has_breakpoint(const char *line);
static void set_widget_background_color(GtkWidget *widget, const char *color);
static void apply_css(GtkWidget *widget, const char *class_name, const char *style);
static void gtk_widget_set_margin_all(GtkWidget *widget, int margin);



/*----------------------- Setup ------------------------*/



/**
 * @brief Launches the gui.
 * @param argc Main's argc.
 * @param argv Main's argv
 * @param computer Pointer to the computer.
 * @return The status of the GUI
 */
int launch_gui(int argc, char **argv, Computer *computer) {
	// The app gets created
	GtkApplication *app = gtk_application_new("org.unican.cachis", G_APPLICATION_DEFAULT_FLAGS);

	// The activate callback gets binded
	g_signal_connect(app, "activate", G_CALLBACK(activate), computer);

	// The app is run
	int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return status;
}

/**
 * @brief Populates the GUI with all the different fields of a computer.
 * @param app The app.
 * @param user_data Pointer to the computer.
 */
static void activate(GtkApplication *app, gpointer user_data) {
	Computer *computer = (Computer *)user_data;

	// A new window gets created with the dimms specified in the header file
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), WINDOW_NAME);
	gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

	// The main container of the gui is created with a padding of 10
	GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_window_set_child(GTK_WINDOW(window), main_box);
	gtk_widget_set_hexpand(main_box, TRUE);
	gtk_widget_set_vexpand(main_box, TRUE);

	/* Inside the main container two horizontal paneds get created. Since paneds only support two elements,
	 * one will contain the right and middle columns and the other one will contain the previous paned and the
	 * left column */
	GtkWidget *left_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
	GtkWidget *right_paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);


	// The three columns get created
	GtkWidget *left_column = create_left_column(computer);
	GtkWidget *middle_section = create_middle_section(computer);
	GtkWidget *right_column = create_right_column(computer);

	// The columns get appended to the panes
	gtk_paned_set_start_child (GTK_PANED(left_paned), left_column);
	gtk_paned_set_end_child (GTK_PANED(left_paned), right_paned);
	gtk_paned_set_start_child (GTK_PANED(right_paned),middle_section);
	gtk_paned_set_end_child (GTK_PANED(right_paned),right_column);


	// By default the leftmost separator gets positioned at 1/4th of the width of the window.
	gtk_paned_set_position (GTK_PANED (left_paned), (WINDOW_WIDTH / 4));

	// Some formatting is done to ensure that handles are grabbable even when collapsed all the way
	gtk_widget_set_margin_end(middle_section, MARGIN_MED);
	gtk_paned_set_wide_handle(GTK_PANED(left_paned), TRUE);
	gtk_paned_set_wide_handle(GTK_PANED(right_paned), TRUE);

	// And the left paned gets added to the main box
	gtk_box_append(GTK_BOX(main_box), left_paned);

	// The main container is displayed
	gtk_window_present(GTK_WINDOW(window));
}


/**
 * @brief Creates the left column.
 * @param computer The computer.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_left_column(Computer *computer) {
	// A new vertical box with spacing 5
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// The topmost toolbar gets created and added
	GtkWidget *toolbar = create_toolbar(computer);
	gtk_box_append(GTK_BOX(box), toolbar);

	// Two boxes get created, one for the trace and one for the stats
	GtkWidget *trace_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget *stats_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	// If no text buffer for the CPU has been created, it gets created
	if (computer->cpu.buffer == NULL) {
		computer->cpu.buffer = gtk_text_buffer_new(NULL);
	}

	// A view of the trace gets created and it gets its properties assigned
	GtkWidget *trace_text = gtk_text_view_new_with_buffer(computer->cpu.buffer);
	computer->cpu.view = GTK_TEXT_VIEW(trace_text);
	gtk_widget_set_margin_all(trace_text, MARGIN_SMALL);
	gtk_widget_set_hexpand(trace_text, TRUE);
	gtk_widget_set_vexpand(trace_text, TRUE);

	// The trace label and text are appended to the box. The trace text is put inside a scrolled window
	GtkWidget *trace_label = gtk_label_new(TXT_TRACE);
	GtkWidget *trace_scroll = gtk_scrolled_window_new();
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (trace_scroll), trace_text);
	gtk_box_append(GTK_BOX(trace_box), trace_label);
	gtk_box_append(GTK_BOX(trace_box), trace_scroll);

	// The stats label and text gets appended to the box
	GtkWidget *stats_label = gtk_label_new(TXT_SIMSTATS);
	GtkWidget *stats_scroll = gtk_scrolled_window_new();
	gtk_box_append(GTK_BOX(stats_box), stats_label);

	// A factory for the stats model is created
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "bind", G_CALLBACK(bind_stats_row), NULL);

	// A wrapper for the statistics_model is created
	GtkSelectionModel *selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(statistics_model)));

	GtkWidget *stats_text = gtk_list_view_new(selection_model, factory);

	gtk_widget_set_size_request(stats_text, 200, 200);
	gtk_widget_set_margin_all(stats_text, MARGIN_SMALL);
	gtk_widget_set_hexpand(stats_text, TRUE);
	gtk_widget_set_vexpand(stats_text, TRUE);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (stats_scroll), stats_text);
	gtk_box_append(GTK_BOX(stats_box), stats_scroll);

	// A paned view gets created. This allows the user to resize the trace / stats views
	GtkWidget *trace_stats_paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_paned_set_start_child (GTK_PANED(trace_stats_paned),trace_box);
	gtk_paned_set_end_child (GTK_PANED(trace_stats_paned),stats_box);

	// By default the separator gets positioned at 2/3 of the height.
	gtk_paned_set_position (GTK_PANED (trace_stats_paned), (WINDOW_HEIGHT / 3) * 2);

	// The pane gets appended to the parent box
	gtk_box_append(GTK_BOX(box), trace_stats_paned);


	return box;
}


/**
 * @brief Creates the middle column.
 * @param computer The computer.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_middle_section(Computer *computer) {
	// GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MARGIN_SMALL);
	GtkWidget *scrolled_window = gtk_scrolled_window_new();
	GtkWidget *paned[MAX_CACHES];

	// For every cache in the hierarchy
	for (int i = 0; i < computer->num_caches; i++) {
		// A paned for this cache gets created
		paned[i] = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

		// A box gets created
		GtkWidget *cache_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MARGIN_SMALL);
		gtk_widget_set_margin_all(cache_box, MARGIN_SMALL);

		// A title gets assigned to the box
		char title[50];
		snprintf(title, 50, "Cache L%d", i + 1);
		GtkWidget *cache_label = gtk_label_new(title);
		gtk_box_append(GTK_BOX(cache_box), cache_label);

		// And a widget showing the cache table is put inside of the box
		GtkWidget *cache_widget = create_cache_widget(&computer->cache[i]);
		gtk_widget_set_hexpand(cache_widget, TRUE);
		gtk_widget_set_vexpand(cache_widget, TRUE);
		gtk_box_append(GTK_BOX(cache_box), cache_widget);

		// The cache is attached to the first element of the paned
		gtk_paned_set_start_child(GTK_PANED(paned[i]), cache_box);

		// Each cache is assigned the proportional part of the middle section, depending on the number of total caches
		gtk_paned_set_position (GTK_PANED(paned[i]), (WINDOW_WIDTH / 2) / (computer->num_caches));

		// If there is more than one cache, this paned has to get attached to the second element of the previous paned
		if (i != 0) {
			gtk_paned_set_end_child(GTK_PANED(paned[i - 1]), paned[i]);
			// gtk_widget_set_margin_start(paned[i], MARGIN_MED);
			gtk_paned_set_wide_handle(GTK_PANED(paned[i - 1]), TRUE);
		}
	}

    gtk_widget_set_size_request (scrolled_window, (WINDOW_WIDTH / 3), -1);
	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (scrolled_window), paned[0]);
	return scrolled_window;
}

/**
 * @brief Creates the right column.
 * @param computer The computer.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_right_column(Computer *computer) {
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	GtkWidget *memory_label = gtk_label_new("Memory");
	gtk_box_append(GTK_BOX(box), memory_label);

	// A memory table gets created
	GtkWidget *memory_table = create_memory_table(computer);
	gtk_widget_set_size_request(memory_table, 200, 400);
	gtk_widget_set_hexpand(memory_table, TRUE);
	gtk_widget_set_vexpand(memory_table, TRUE);
	gtk_box_append(GTK_BOX(box), memory_table);

	return box;
}


/**
 * @brief Creates the mini toolbar on the leftmost column.
 * @param computer The computer.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_toolbar(Computer *computer) {
	GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_add_css_class(toolbar, "toolbar");

	GtkWidget *open_button = gtk_button_new_from_icon_name("document-open");
	gtk_widget_set_tooltip_text(open_button, "Open Trace File");
	gtk_box_append(GTK_BOX(toolbar), open_button);

	GtkWidget *run_button = gtk_button_new_from_icon_name("media-playback-start");
	gtk_widget_set_tooltip_text(run_button, "Run Simulation");
	gtk_box_append(GTK_BOX(toolbar), run_button);

	GtkWidget *step_button = gtk_button_new_from_icon_name("media-skip-forward");
	gtk_widget_set_tooltip_text(step_button, "Step Simulation");
	gtk_box_append(GTK_BOX(toolbar), step_button);

	GtkWidget *reset_button = gtk_button_new_from_icon_name("view-refresh");
	gtk_widget_set_tooltip_text(reset_button, "Reset Simulation");
	gtk_box_append(GTK_BOX(toolbar), reset_button);

	g_signal_connect(step_button, "clicked", G_CALLBACK(on_step_button_clicked), computer);
	g_signal_connect(run_button, "clicked", G_CALLBACK(on_run_to_breakpoint_clicked), computer);
	g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_button_clicked), computer);

	return toolbar;
}



/**
 * @brief Creates a cache widget containing the cache tables.
 * @param cache A pointer to the cache that will be used to create the cache widget.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_cache_widget(Cache *cache) {
	GtkWidget *cache_label;

	// If the caches are separated, two tables have to be created on the same box
	if (cache->separated) {
		// Two boxes (One for each cache) and a paned are created
		GtkWidget *top_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

		// A label is added for the data and instruction caches
		cache_label = gtk_label_new(C_SEP_DATA);
		gtk_box_append(GTK_BOX(top_box), cache_label);

		// The tables are created based on the data model that is provided and the view gets stored in the view_data pointer
		GtkWidget *data_table = create_cache_table(G_LIST_STORE(cache->model_data));
		cache->view_data = data_table;
		gtk_widget_set_vexpand(data_table, TRUE);
		gtk_box_append(GTK_BOX(top_box), data_table);

		// Same for the instruction cache
		cache_label = gtk_label_new(C_SEP_INST);
		gtk_box_append(GTK_BOX(bottom_box), cache_label);
		GtkWidget *instruction_table = create_cache_table(G_LIST_STORE(cache->model_instruction));
		cache->view_instruction = instruction_table;
		gtk_widget_set_vexpand(instruction_table, TRUE);
		gtk_box_append(GTK_BOX(bottom_box), instruction_table);

		gtk_paned_set_start_child(GTK_PANED(paned), top_box);
		gtk_paned_set_end_child(GTK_PANED(paned), bottom_box);
		return paned;
	} else {
		GtkWidget *unified_table = create_cache_table(G_LIST_STORE(cache->model_data));
		cache->view_data = unified_table;
		gtk_widget_set_vexpand(unified_table, TRUE);
		return unified_table;
	}
}


/**
 * @brief Creates a cache table with all columns inside of a scrolled window.
 * @param model A pointer to the model of the cache.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_cache_table(GListStore *model) {
	// A new scrolled window with automatic policies gets created
	GtkWidget *scrolled_window = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	// A single selection and a column view gets created and a pointer to the single selection is also kept
	GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(model));
	GtkWidget *column_view = gtk_column_view_new(GTK_SELECTION_MODEL(selection));

	// A list item factory is created. This will manage the visual representation of the data.
	GtkListItemFactory *factory;

	// Line
	// A new factory gets created for this column
	factory = gtk_signal_list_item_factory_new();

	// The setup_cache and bind callbacks are executed upon creation
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_line_cb), NULL);

	// A new column with this factory is created and appended to the column view
	GtkColumnViewColumn *column = gtk_column_view_column_new(C_LINE, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Set
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_set_cb), NULL);
	column = gtk_column_view_column_new(C_SET, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Valid
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_valid_cb), NULL);
	column = gtk_column_view_column_new(C_VALID, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Dirty
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_dirty_cb), NULL);
	column = gtk_column_view_column_new(C_DIRTY, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Accessed
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_accessed_cb), NULL);
	column = gtk_column_view_column_new(C_ACC, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Last Access
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_last_access_cb), NULL);
	column = gtk_column_view_column_new(C_LASTACC, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// First Access
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_first_access_cb), NULL);
	column = gtk_column_view_column_new(C_FIRSTACC, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Tag
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_tag_cb), NULL);
	column = gtk_column_view_column_new(C_TAG, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	// Content
	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_cache_content_cb), NULL);
	column = gtk_column_view_column_new(C_CONTENT, factory);
	gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
	g_object_unref(column);

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), column_view);
	return scrolled_window;
}


/**
 * @brief Creates a cache memory table
 * @param computer The computer that has the memory struct.
 * @return A pointer to the created widget.
 */
static GtkWidget *create_memory_table(Computer *computer) {
	// A new scrolled window with default policies is created
	GtkWidget *scrolled_window = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	// A single selection and column view get created
	GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(computer->memory.model));
	gtk_single_selection_set_autoselect(selection,TRUE);
	GtkWidget *column_view = gtk_column_view_new(GTK_SELECTION_MODEL (selection));
	gtk_widget_set_margin_all(column_view, MARGIN_MED);

	// Like in the caches, a factory is created to maintain the view and binds for the address and content are called upon creation
	GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb),NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_address_cb),NULL);
	GtkColumnViewColumn *column = gtk_column_view_column_new(M_ADDR, factory);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
	g_object_unref (column);

	factory = gtk_signal_list_item_factory_new();
	g_signal_connect(factory, "setup", G_CALLBACK(setup_cb),NULL);
	g_signal_connect(factory, "bind", G_CALLBACK(bind_content_cb),NULL);
	column = gtk_column_view_column_new(M_CONT, factory);
	gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
	g_object_unref (column);

	// The column view is appended to the scrolled view, a pointer to the view is saved and returned
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), column_view);
	computer->memory.view = column_view;
	return scrolled_window;
}



/*----------------------- Binds ------------------------*/



/**
 * @brief Setup for the memory view.
 * @param factory
 * @param listitem
 */
static void setup_cb(GtkSignalListItemFactory *factory, GObject *listitem) {
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	GtkWidget *label = gtk_label_new(NULL);
	gtk_box_append(GTK_BOX(box), label);
	gtk_list_item_set_child(GTK_LIST_ITEM(listitem), box);
}

/**
 * @brief Populates the address field in the memory view.
 * @param factory
 * @param listitem
 */
static void bind_address_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	// The elements of the cell get created
	GtkWidget *box = gtk_list_item_get_child(listitem);
	GtkWidget *label = gtk_widget_get_first_child(box);
	MemoryLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

	// A string with the address gets created
	char *string = g_strdup_printf("0x%x", item->address);
	gtk_label_set_text(GTK_LABEL(label), string);
	g_free(string);

	// The color of the background is set to be the same as the item's.
	if (item->color) {
		set_widget_background_color(box, item->color);
	}
}

/**
 * @brief Populates the content field in the memory view.
 * @param factory
 * @param listitem
 */
static void bind_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	// The elements of the cell get created
	GtkWidget *box = gtk_list_item_get_child(listitem);
	GtkWidget *label = gtk_widget_get_first_child(box);
	MemoryLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

	// The content is converted to hex and displayed
	char *string = g_strdup_printf("0x%x", item->content);
	gtk_label_set_text(GTK_LABEL(label), string);
	g_free(string);

	// The color of the background is set to be the same as the item's.
	if (item->color) {
		set_widget_background_color(box, item->color);
	}
}


/**
 * @brief Setup for the cache view.
 * @param factory
 * @param listitem
 */
static void setup_cache_cb(GtkSignalListItemFactory *factory, GObject *listitem) {
	GtkWidget *label = gtk_label_new(NULL);
	gtk_list_item_set_child(GTK_LIST_ITEM(listitem), label);
}

/**
 * @brief Populates the line field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_line_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	// The required elements for the cell get created
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));

	// The line is displayed as an unsigned integer and set
	char *string = g_strdup_printf("%u", item->line);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the set field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_set_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->set);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the valid bit field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_valid_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->valid);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the dirty bit field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_dirty_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->dirty);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the number of accesses field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_accessed_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->times_accessed);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the last access field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_last_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->last_accessed);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the first access field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_first_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("%u", item->first_accessed);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the tag field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_tag_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	char *string = g_strdup_printf("0x%x", item->tag);
	gtk_label_set_text(GTK_LABEL(label), string);
}

/**
 * @brief Populates the content field in the cache view.
 * @param factory
 * @param listitem
 */
static void bind_cache_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
	GtkWidget *label = gtk_list_item_get_child(listitem);
	CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
	gtk_label_set_text(GTK_LABEL(label), item->content_cache);
}

/**
 * @brief Creates a row of statistics.
 * @param factory
 * @param list_item
 */
static void bind_stats_row(GtkListItemFactory *factory, GtkListItem *list_item) {
	// Retrieve the StatsNode associated with this list item
    StatsNode *data = gtk_list_item_get_item(GTK_LIST_ITEM(list_item));

	if (data == NULL) {
        return;
    }

	// If it's a component
    if (data->isComponent) {
		// A new expander gets created
		GtkWidget *expander = gtk_expander_new(data->name);
		gtk_list_item_set_child(list_item, expander);

		// The pointer to the expander is saved
		data->container = expander;

		// The state of the expander is updated.
		gtk_expander_set_expanded(GTK_EXPANDER(expander), data->isExpanded);

		// A the expand signal is connected to on_expander_toggled. It keeps the state of the extender after updating the model.
		// This avoids the expander closing if it has been previously opened
        g_signal_connect(expander, "notify::expanded", G_CALLBACK(on_expander_toggled), data);
	} else {
		// If the node is a property, a box with the name and content are created
        GtkWidget *name = gtk_label_new(data->name);
        GtkWidget *content = gtk_label_new(data->content);
		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, MARGIN_SMALL);

		// The pointer to the box is saved
		data->container = box;

		// A large left margin gets applied and the labels are appended to the box
		gtk_widget_set_margin_start(name, MARGIN_LARGE);
		gtk_box_append(GTK_BOX(box), name);
		gtk_box_append(GTK_BOX(box), content);


		// The new box is appended
		gtk_list_item_set_child(list_item, box);

		if (!data->isExpanded) {
			g_object_set(box, "visible", FALSE, NULL);
		} else {
			g_object_set(box, "visible", TRUE, NULL);
		}
    }
}


/**
 * @brief Toggles the state of the node in the model after the expander gets clicked.
 * @param expander The expander.
 * @param pspec
 * @param data Pointer to the StatsNode that has the expander.
 */
static void on_expander_toggled(GtkExpander *expander, GParamSpec *pspec, StatsNode *data) {
	// The state is toggled
    data->isExpanded = !data->isExpanded;

	// The model is fetched
    GListModel *model = gtk_tree_list_model_get_model(statistics_model);
	guint num_children = g_list_model_get_n_items(model);
	StatsNode *comp_node, *prop_node;

	// All the components are iterated
	for (guint i = 0; i < num_children; i++) {
		comp_node = g_list_model_get_item(model, i);

		// If the is the same as the one that has been expanded
		if (g_strcmp0(comp_node->name, data->name) == 0) {
			int num_properties = g_list_model_get_n_items(G_LIST_MODEL(comp_node->children));

			// All the children are collapsed
			for (guint j = 0; j < num_properties; j++) {
				prop_node = g_list_model_get_item(G_LIST_MODEL(comp_node->children), j);

				prop_node->isExpanded = !prop_node->isExpanded;

				// The visibility of the children is changed
				if (prop_node->isExpanded) {
					g_object_set(prop_node->container, "visible", TRUE, NULL);
				} else {
					g_object_set(prop_node->container, "visible", FALSE, NULL);
				}
			}
		}
	}
}

/**
 * @brief Executes until a breakpoint is found or the simulation is finished.
 * @param button
 * @param computer The computer.
 */
static void on_run_to_breakpoint_clicked(GtkButton *button, Computer *computer) {
	// A buffer with the instructions and iterators are created
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(computer->cpu.view);
	GtkTextIter start, end;

	// If nothing has been highlighted yet, a new background highlight is created
	if (highlight_tag == NULL) {
		highlight_tag = gtk_text_buffer_create_tag(buffer, "highlight", "background", HIGHLIGHT_COLOR, NULL);
	}

	// If something was previously highlighted, it is removed
	if (previous_highlight_mark != NULL) {
		GtkTextIter prev_start, prev_end;
		// Get the previous highlight
		gtk_text_buffer_get_iter_at_mark(buffer, &prev_start, previous_highlight_mark);
		prev_end = prev_start;

		// This has to be done to set a different end
		gtk_text_iter_forward_line(&prev_end);

		// The highlight is removed
		gtk_text_buffer_remove_tag(buffer, highlight_tag, &prev_start, &prev_end);
	}

	// If nothing has been previously highlighted
	if (previous_highlight_mark == NULL) {
		// An iterator at the beginning the buffer is saved
		gtk_text_buffer_get_start_iter(buffer, &start);
	} else {
		// An iterator at the previously highlighted position is saved and moved to the next step
		gtk_text_buffer_get_iter_at_mark(buffer, &start, previous_highlight_mark);
		gtk_text_iter_forward_line(&start);
	}

	gboolean breakpoint_found = FALSE;

	// While there is no breakpoint and there's still trace left to execute
	while (!breakpoint_found && !gtk_text_iter_is_end(&start)) {
		end = start;
		// The end pointer is moved to the end of the line
		gtk_text_iter_forward_line(&end);

		// The whole line is read (From start to end)
		gchar *line_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

		// If the text is ok,
		if (line_text && *line_text) {
			// Check for breakpoints
			if(has_breakpoint(line_text)) {
				breakpoint_found = TRUE;
			} else {
				// If there are no breakpoints, move on to the next line
				step_trace_line(line_text, computer);
			}
		}

		// Free the text
		g_free(line_text);

		// If a breakpoint is found, end the loop and stop the simulation
		if (!breakpoint_found) {
			start = end;
		}
	}

	// The highlight is moved to the new line
	gtk_text_buffer_apply_tag(buffer, highlight_tag, &start, &end);

	// If there was no previous highlight, it gets noted
	if (previous_highlight_mark == NULL) {
		previous_highlight_mark = gtk_text_buffer_create_mark(buffer, "previous_highlight", &start, TRUE);
	} else {
		// If there was, it gets updated
		gtk_text_buffer_move_mark(buffer, previous_highlight_mark, &start);
	}

	// The text is scrolled to that line
	gtk_text_view_scroll_to_iter(computer->cpu.view, &start, 0.0, TRUE, 0.0, 0.5);
}

/**
 * @brief Runs a single step when the step button is clicked.
 * @param button
 * @param computer The computer.
 */
static void on_step_button_clicked(GtkButton *button, Computer *computer) {
	// View comments for on_run_to_breakpoint_clicked
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(computer->cpu.view);
	GtkTextIter start, end;
 
	if (highlight_tag == NULL) {
		highlight_tag = gtk_text_buffer_create_tag(buffer, "highlight", "background", HIGHLIGHT_COLOR, NULL);
	}

	if (previous_highlight_mark != NULL) {
		GtkTextIter prev_start, prev_end;
		gtk_text_buffer_get_iter_at_mark(buffer, &prev_start, previous_highlight_mark);
		prev_end = prev_start;
		gtk_text_iter_forward_line(&prev_end);
		gtk_text_buffer_remove_tag(buffer, highlight_tag, &prev_start, &prev_end);
	}

	if (previous_highlight_mark == NULL) {
		gtk_text_buffer_get_start_iter(buffer, &start);
	} else {
		gtk_text_buffer_get_iter_at_mark(buffer, &start, previous_highlight_mark);
		gtk_text_iter_forward_line(&start);
	}

	gboolean valid_line_found = FALSE;
	while (!valid_line_found && !gtk_text_iter_is_end(&start)) {
		end = start;
		gtk_text_iter_forward_line(&end);

		gchar *line_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

		if (line_text && *line_text) {
			gboolean error = step_trace_line(line_text, computer);
			// If the execution was correct
			if (!error) {
				// The simulation is stopped (Single step) and the highlights are applied
				valid_line_found = TRUE;
				gtk_text_buffer_apply_tag(buffer, highlight_tag, &start, &end);
				if (previous_highlight_mark == NULL) {
					previous_highlight_mark = gtk_text_buffer_create_mark(buffer, "previous_highlight", &start, TRUE);
				} else {
					gtk_text_buffer_move_mark(buffer, previous_highlight_mark, &start);
				}
			}
		}
		g_free(line_text);

		if (!valid_line_found) {
			start = end;
		}
	}

	gtk_text_view_scroll_to_iter(computer->cpu.view, &start, 0.0, TRUE, 0.0, 0.5);
}

/**
 * @brief Resets the simulation when the reset button is clicked.
 * @param button
 * @param computer The computer.
 */
static void on_reset_button_clicked(GtkButton *button, Computer *computer) {
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(computer->cpu.view);

	// If there was no previous highlight, it gets searched
	if (highlight_tag == NULL) {
		GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
		highlight_tag = gtk_text_tag_table_lookup(tag_table, "highlight");
	}

	if (previous_highlight_mark != NULL) {
		GtkTextIter start, end;
		gtk_text_buffer_get_iter_at_mark(buffer, &start, previous_highlight_mark);
		end = start;
		gtk_text_iter_forward_line(&end);
		gtk_text_buffer_remove_tag(buffer, highlight_tag, &start, &end);

		gtk_text_buffer_delete_mark(buffer, previous_highlight_mark);
		previous_highlight_mark = NULL;
	}

	GtkTextIter start;
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_view_scroll_to_iter(computer->cpu.view, &start, 0.0, TRUE, 0.0, 0.5);

	// The memory, cache and statistics models are reset
	reset_memory_model(computer);
	reset_cache_model(computer);
	reset_statistics_model(computer);

	// The cycle is reset to 0
	cycle = 0;
}


void print_error_message (const char *message, int line_number) {
	g_printerr ("%s Line %d\n", message, line_number);
}



/*----------------------- Misc ------------------------*/



/**
 * @brief Simulates the line passed as an argument.
 * @param line Pointer to the line's content.
 * @param computer The computer
 * @return 0 if completed sucessfully, 1 if error.
 */
int step_trace_line(char *line, Computer *computer) {
	MemoryOperation operation;

	// The line gets preprocessed to remove comments
	if (!preprocessTraceLine(line)) {
		return 1;
	}

   // The line gets parsed and all of the information gets stored in the operation
	if (parseLine(line, -1, &operation, computer->cpu.word_width/8, &computer->memory) == -1) {
		return 1;
	}

	// The operation gets simulated
	simulate_step(computer, &operation);
	return 0;
}

/**
 * @brief Checks for breakpoints (!) in the current line.
 * @param line Pointer to the line.
 * @return 1 If the line has a breakpoint, 0 if not
 */
int has_breakpoint(const char *line) {
   // Check if line has a ! at the beginning ignoring leading whitespace
	while (isspace(*line)) {
		line++;
	}
	return *line == '!';
}


/**
 * @brief Changes the background color of a widget.
 * @param widget Pointer to the widget that should get the style applied
 * @param color The color that should be used
 */
static void set_widget_background_color(GtkWidget *widget, const char *color) {
	// A CSS provider is created
	GtkCssProvider *provider = gtk_css_provider_new();

	// A CSS class is defined and applied
	char *css = g_strdup_printf(".bg-color { background-color: %s; }", color);
	apply_css(widget, "bg-color", css);

	g_free(css);
	g_object_unref(provider);
}

/**
 * @brief Applies some CSS class to the specified widget
 * @param widget Pointer to the widget that should get the style applied
 * @param class_name The name of the class to apply to the widget
 * @param style The CSS class definition with the style and the class name ".example { background-color: red; }"
 */
static void apply_css(GtkWidget *widget, const char *class_name, const char *style) {
	// A CSS provider is created
	GtkCssProvider *provider = gtk_css_provider_new();

	// A CSS class is defined and attached to the provider
	gtk_css_provider_load_from_string(provider, style);

	// A display for the widget is created and the CSS provider is attached
	GdkDisplay *display = gtk_widget_get_display(widget);
    gtk_style_context_add_provider_for_display(
        display,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

	// The widget is assigned the previously created widget class
	gtk_widget_add_css_class(widget, class_name);
}


/**
 * @brief Sets all the margins to the same value at once. GTK does not offer this by default.
 * @param widget Pointer to the widget to set the margins.
 * @param margin Size of the margin.
 */
static void gtk_widget_set_margin_all(GtkWidget *widget, int margin) {
	gtk_widget_set_margin_bottom(widget, margin);
	gtk_widget_set_margin_top(widget, margin);
	gtk_widget_set_margin_start(widget, margin);
	gtk_widget_set_margin_end(widget, margin);
}
