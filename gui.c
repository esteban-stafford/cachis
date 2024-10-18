#include "gui.h"

static GtkApplication *app = NULL;

static void activate(GtkApplication *app, gpointer user_data) {
    Computer *computer = (Computer *)user_data;

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cache Simulation");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Trace File and Simulation Stats (Left Column)
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

    // Middle section (L1 and L2 Cache Tables)
    for (int i = 0; i < computer->num_caches; i++) {
        if (computer->cache[i].separated) {
            GtkWidget *l1_table = gtk_label_new("Cache L1"); // Placeholder for L1 Table
            gtk_widget_set_size_request(l1_table, 150, 200);
            gtk_grid_attach(GTK_GRID(grid), l1_table, i + 1, 1, 1, 1);

            GtkWidget *l2_table = gtk_label_new("Cache L2"); // Placeholder for L2 Table
            gtk_widget_set_size_request(l2_table, 150, 200);
            gtk_grid_attach(GTK_GRID(grid), l2_table, i + 1, 2, 1, 1);
        } else {
            GtkWidget *cache_table = gtk_label_new("Unified Cache"); // Placeholder for Unified Cache Table
            gtk_widget_set_size_request(cache_table, 150, 400);
            gtk_grid_attach(GTK_GRID(grid), cache_table, i + 1, 1, 1, 2);
        }
    }

    // Memory (Right Column)
    GtkWidget *memory_label = gtk_label_new("Memory");
    gtk_grid_attach(GTK_GRID(grid), memory_label, computer->num_caches + 1, 0, 1, 1);

    GtkWidget *memory_text = gtk_text_view_new();
    gtk_widget_set_size_request(memory_text, 200, 400);
    gtk_grid_attach(GTK_GRID(grid), memory_text, computer->num_caches + 1, 1, 1, 5);

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

