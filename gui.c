#include <ctype.h>
#include "gui.h"
#include "datastore.h"
#include "traceparser.h"
#include "simulator.h"

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
    computer->memory.view = column_view;
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

static GtkWidget *create_cache_table(GListStore *model) {
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
    if (cache->separated) {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

        GtkWidget *data_table = create_cache_table(G_LIST_STORE(cache->model_data));
        gtk_widget_set_vexpand(data_table, TRUE); // Ensure vertical expansion for data_table
        gtk_box_append(GTK_BOX(box), data_table);

        GtkWidget *instruction_table = create_cache_table(G_LIST_STORE(cache->model_instruction));
        gtk_widget_set_vexpand(instruction_table, TRUE); // Ensure vertical expansion for instruction_table
        gtk_box_append(GTK_BOX(box), instruction_table);

        return box;
    } else {
        GtkWidget *unified_table = create_cache_table(G_LIST_STORE(cache->model_data));
        gtk_widget_set_vexpand(unified_table, TRUE); // Ensure vertical expansion for unified cache
        return unified_table;
    }
}

int step_trace_line(char *line, Computer *computer) {
   if(!preprocessTraceLine(line)) {
      return 1;
   }
   struct memOperation operation;
   if(parseLine(line, -1, &operation, computer->cpu.word_width/8, &computer->memory) == -1) {
        return 1;
   }
   simulate_step(computer, &operation);
   return 0;
}

int has_breakpoint(const char *line) {
   // Check if line has a ! at the beginning ignoring leading whitespace
   while(isspace(*line)) {
      line++;
   }
   return *line == '!';
}

static GtkTextTag *highlight_tag = NULL;
static GtkTextMark *previous_highlight_mark = NULL;

struct trace_text_and_computer {
    GtkTextView *trace_text;
    Computer *computer;
};

static void on_run_to_breakpoint_clicked(GtkButton *button, gpointer user_data) {
    struct trace_text_and_computer *data = (struct trace_text_and_computer *)user_data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(data->trace_text);
    GtkTextIter start, end;

    if (highlight_tag == NULL) {
        highlight_tag = gtk_text_buffer_create_tag(buffer, "highlight", "background", "#219ebc", NULL);
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

    gboolean breakpoint_found = FALSE;
    while (!breakpoint_found && !gtk_text_iter_is_end(&start)) {
        end = start;
        gtk_text_iter_forward_line(&end);

        gchar *line_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

        if (line_text && *line_text) {
            if(has_breakpoint(line_text)) {
               breakpoint_found = TRUE;
            } else {
                step_trace_line(line_text, data->computer);
            }
        }
        g_free(line_text);

        if (!breakpoint_found) {
            start = end;
        }
    }

    gtk_text_buffer_apply_tag(buffer, highlight_tag, &start, &end);
    if (previous_highlight_mark == NULL) {
        previous_highlight_mark = gtk_text_buffer_create_mark(buffer, "previous_highlight", &start, TRUE);
    } else {
        gtk_text_buffer_move_mark(buffer, previous_highlight_mark, &start);
    }

    gtk_text_view_scroll_to_iter(data->trace_text, &start, 0.0, TRUE, 0.0, 0.5);
}

static void on_step_button_clicked(GtkButton *button, gpointer user_data) {
    struct trace_text_and_computer *data = (struct trace_text_and_computer *)user_data;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(data->trace_text);
    GtkTextIter start, end;
 
    if (highlight_tag == NULL) {
        highlight_tag = gtk_text_buffer_create_tag(buffer, "highlight", "background", "#219ebc", NULL);
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
            gboolean result = step_trace_line(line_text, data->computer);
            if (!result) {
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

    gtk_text_view_scroll_to_iter(data->trace_text, &start, 0.0, TRUE, 0.0, 0.5);
}


static void on_reset_button_clicked(GtkButton *button, GtkTextView *trace_text) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(trace_text);

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
    gtk_text_view_scroll_to_iter(trace_text, &start, 0.0, TRUE, 0.0, 0.5);
}

static GtkWidget *create_toolbar(GtkTextView *trace_text, Computer *computer) {
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

    struct trace_text_and_computer *data = g_new(struct trace_text_and_computer, 1);
    data->trace_text = trace_text;
    data->computer = computer;

    g_signal_connect(step_button, "clicked", G_CALLBACK(on_step_button_clicked), data);
    g_signal_connect(run_button, "clicked", G_CALLBACK(on_run_to_breakpoint_clicked), data);
    g_signal_connect(reset_button, "clicked", G_CALLBACK(on_reset_button_clicked), trace_text);

    return toolbar;
}

static GtkWidget *create_left_column(Computer *computer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    if (computer->cpu.buffer == NULL) {
        computer->cpu.buffer = gtk_text_buffer_new(NULL);
    }

    GtkWidget *trace_text = gtk_text_view_new_with_buffer(computer->cpu.buffer);
    gtk_widget_set_size_request(trace_text, 200, 400);
    gtk_widget_set_hexpand(trace_text, TRUE);
    gtk_widget_set_vexpand(trace_text, TRUE);

    GtkWidget *toolbar = create_toolbar(GTK_TEXT_VIEW(trace_text), computer);
    gtk_box_append(GTK_BOX(box), toolbar);

    GtkWidget *trace_label = gtk_label_new("trace file: filename.vca");
    gtk_box_append(GTK_BOX(box), trace_label);

    gtk_box_append(GTK_BOX(box), trace_text);

    GtkWidget *stats_label = gtk_label_new("simulation statistics");
    gtk_box_append(GTK_BOX(box), stats_label);

    GtkWidget *stats_text = gtk_text_view_new();
    gtk_widget_set_size_request(stats_text, 200, 200);
    gtk_widget_set_hexpand(stats_text, TRUE);
    gtk_widget_set_vexpand(stats_text, TRUE);
    gtk_box_append(GTK_BOX(box), stats_text);

    return box;
}


static GtkWidget *create_middle_section(Computer *computer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    for (int i = 0; i < computer->num_caches; i++) {
        GtkWidget *cache_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

        char title[50];
        snprintf(title, 50, "Cache L%d", i + 1);
        GtkWidget *cache_label = gtk_label_new(title);
        gtk_box_append(GTK_BOX(cache_box), cache_label);

        GtkWidget *cache_widget = create_cache_widget(&computer->cache[i], i + 1);
        gtk_widget_set_size_request(cache_widget, 150, 400);
        gtk_widget_set_hexpand(cache_widget, TRUE);
        gtk_widget_set_vexpand(cache_widget, TRUE);
        gtk_box_append(GTK_BOX(cache_box), cache_widget);

        gtk_box_append(GTK_BOX(box), cache_box);
    }

    return box;
}

static GtkWidget *create_right_column(Computer *computer) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *memory_label = gtk_label_new("Memory");
    gtk_box_append(GTK_BOX(box), memory_label);

    GtkWidget *memory_table = create_memory_table(computer);
    gtk_widget_set_size_request(memory_table, 200, 400);
    gtk_widget_set_hexpand(memory_table, TRUE);
    gtk_widget_set_vexpand(memory_table, TRUE);
    gtk_box_append(GTK_BOX(box), memory_table);

    return box;
}

static void activate(GtkApplication *app, gpointer user_data) {
    Computer *computer = (Computer *)user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cache Simulation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), main_box);
    gtk_widget_set_hexpand(main_box, TRUE);
    gtk_widget_set_vexpand(main_box, TRUE);

    GtkWidget *left_column = create_left_column(computer);
    gtk_box_append(GTK_BOX(main_box), left_column);

    GtkWidget *middle_section = create_middle_section(computer);
    gtk_box_append(GTK_BOX(main_box), middle_section);

    GtkWidget *right_column = create_right_column(computer);
    gtk_box_append(GTK_BOX(main_box), right_column);

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

void print_error_message (const char *message, int line_number)
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

