#include "gui.h"
#include "datastore.h"

static GtkApplication *app = NULL;

static void setup_cb(GtkSignalListItemFactory *factory,GObject  *listitem) {
    GtkWidget *label =gtk_label_new(NULL);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem),label);
}

static void bind_address_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem)
{
    GtkWidget *label = gtk_list_item_get_child(listitem);
    MemoryLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%d", item->address);
    gtk_label_set_text(GTK_LABEL (label), string);
}

static void bind_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem)
{
    GtkWidget *label = gtk_list_item_get_child(listitem);
    MemoryLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%d", item->content);
    gtk_label_set_text(GTK_LABEL (label), string);
}

static GtkWidget *create_memory_table(Computer *computer) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(computer->memory.model));
    gtk_single_selection_set_autoselect(selection,TRUE);
    GtkWidget *column_view = gtk_column_view_new(GTK_SELECTION_MODEL (selection));

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cb),NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_address_cb),NULL);
    GtkColumnViewColumn *column = gtk_column_view_column_new("Address", factory);
    gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
    g_object_unref (column);

    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cb),NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_content_cb),NULL);
    column = gtk_column_view_column_new("Content", factory);
    gtk_column_view_append_column (GTK_COLUMN_VIEW (column_view), column);
    g_object_unref (column);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), column_view);
    return scrolled_window;
}

static void setup_cache_cb(GtkSignalListItemFactory *factory, GObject *listitem) {
    GtkWidget *label = gtk_label_new(NULL);
    gtk_list_item_set_child(GTK_LIST_ITEM(listitem), label);
}

static void bind_line_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->line);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_set_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->set);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_valid_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->valid);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_dirty_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->dirty);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_accessed_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->times_accessed);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_last_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->last_accessed);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_first_access_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->first_accessed);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_tag_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    char *string = g_strdup_printf("%u", item->tag);
    gtk_label_set_text(GTK_LABEL(label), string);
}

static void bind_cache_content_cb(GtkSignalListItemFactory *factory, GtkListItem *listitem) {
    GtkWidget *label = gtk_list_item_get_child(listitem);
    CacheLine *item = gtk_list_item_get_item(GTK_LIST_ITEM(listitem));
    gtk_label_set_text(GTK_LABEL(label), item->content_cache);
}

static GtkWidget *create_cache_table(GListStore *model, const char *title) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkSingleSelection *selection = gtk_single_selection_new(G_LIST_MODEL(model));
    GtkWidget *column_view = gtk_column_view_new(GTK_SELECTION_MODEL(selection));

    GtkListItemFactory *factory;

    // Line
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_line_cb), NULL);
    GtkColumnViewColumn *column = gtk_column_view_column_new("Line", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Set
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_set_cb), NULL);
    column = gtk_column_view_column_new("Set", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Valid
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_valid_cb), NULL);
    column = gtk_column_view_column_new("Valid", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Dirty
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_dirty_cb), NULL);
    column = gtk_column_view_column_new("Dirty", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Accessed
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_accessed_cb), NULL);
    column = gtk_column_view_column_new("Accessed", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Last Access
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_last_access_cb), NULL);
    column = gtk_column_view_column_new("Last Access", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // First Access
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_first_access_cb), NULL);
    column = gtk_column_view_column_new("First Access", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Tag
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_tag_cb), NULL);
    column = gtk_column_view_column_new("Tag", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    // Content
    factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_cache_cb), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_cache_content_cb), NULL);
    column = gtk_column_view_column_new("Content", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(column_view), column);
    g_object_unref(column);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), column_view);
    return scrolled_window;
}

static GtkWidget *create_cache_widget(Cache *cache, int level) {
    char title[50];

    if (cache->separated) {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

        snprintf(title, 50, "Cache L%d Data", level);
        GtkWidget *data_table = create_cache_table(G_LIST_STORE(cache->model_data), title);
        gtk_box_append(GTK_BOX(box), data_table);

        snprintf(title, 50, "Cache L%d Instruction", level);
        GtkWidget *instruction_table = create_cache_table(G_LIST_STORE(cache->model_instruction), title);
        gtk_box_append(GTK_BOX(box), instruction_table);

        return box;
    } else {
        snprintf(title, 50, "Cache L%d", level);
        return create_cache_table(G_LIST_STORE(cache->model_data), title);
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    Computer *computer = (Computer *)user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cache Simulation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Left Column
    GtkWidget *trace_label = gtk_label_new("trace file: filename.vca");
    gtk_grid_attach(GTK_GRID(grid), trace_label, 0, 0, 1, 1);

    GtkWidget *trace_text = gtk_text_view_new();
    gtk_widget_set_size_request(trace_text, 200, 400);
    gtk_grid_attach(GTK_GRID(grid), trace_text, 0, 1, 1, 5);

    GtkWidget *stats_label = gtk_label_new("simulation statistics");
    gtk_grid_attach(GTK_GRID(grid), stats_label, 0, 6, 1, 1);

    GtkWidget *stats_text = gtk_text_view_new();
    gtk_widget_set_size_request(stats_text, 200, 200);
    gtk_grid_attach(GTK_GRID(grid), stats_text, 0, 7, 1, 5);

    // Middle Section
    for (int i = 0; i < computer->num_caches; i++) {
       GtkWidget *cache_widget = create_cache_widget(&computer->cache[i], i + 1);
       gtk_widget_set_size_request(cache_widget, 150, 400);
       gtk_grid_attach(GTK_GRID(grid), cache_widget, i + 1, 1, 1, computer->cache[i].separated ? 2 : 1);
    }

    // Right Column (Memory Table)
    GtkWidget *memory_label = gtk_label_new("Memory");
    gtk_grid_attach(GTK_GRID(grid), memory_label, computer->num_caches + 1, 0, 1, 1);

    GtkWidget *memory_table = create_memory_table(computer);
    gtk_grid_attach(GTK_GRID(grid), memory_table, computer->num_caches + 1, 1, 1, 5);

    for (int i = 0; i <= computer->num_caches + 1; i++) {
        gtk_widget_set_hexpand(gtk_grid_get_child_at(GTK_GRID(grid), i, 1), TRUE);
    }

    gtk_window_present(GTK_WINDOW(window));
}


int launch_gui(int argc, char **argv, Computer *computer) {
    GtkApplication *app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);

    g_signal_connect(app, "activate", G_CALLBACK(activate), computer);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void on_dialog_response(GtkDialog *dialog,
      int        response_id,
      gpointer   user_data)
{
   gtk_window_destroy (GTK_WINDOW (dialog));
}

void print_error_message (const char *message,
      int         line_number)
{
   if (app == NULL)
   {
      g_printerr ("%s Line %d\n", message, line_number);
      return;
   }

   GtkWindow *window = gtk_application_get_active_window(app);

   GtkWidget *dialog;
   dialog = gtk_message_dialog_new(GTK_WINDOW (window),
         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
         GTK_MESSAGE_ERROR,
         GTK_BUTTONS_CLOSE,
         "%s\nLine %d", message, line_number);

   g_signal_connect(dialog, "response", G_CALLBACK (on_dialog_response), NULL);

   gtk_widget_show(dialog);
}

