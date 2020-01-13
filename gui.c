#include <math.h>
#include "gui.h"
#include "datainterface.h"
#include "datastore.h"
#include "callbacks.h"

int useGUI=1;

/**
 * Function to create the GUI. Data structures for the gui tables must have been created previously.
 */
int generateGUI(int argc, char *argv[]) {
   if(!useGUI) {
      return 0;
   }
   //esto se usara mas adelante. Es para colorear texts.
   tagBlue = gtk_text_buffer_create_tag (buffer, "blue_foreground", "foreground", "blue", NULL);
   tagBlack = gtk_text_buffer_create_tag (buffer, "black_foreground", "foreground", "black", NULL);
   //Esto tabién se usara mas adelante es para recorrer el buffer de text del file trace
   gtk_text_buffer_get_iter_at_offset (buffer, &lineIterInicial, 0);
   marcaLineCurrent= gtk_text_mark_new (NULL, 1);
   gtk_text_buffer_add_mark (buffer,
         marcaLineCurrent,
         &lineIterInicial);
   /* This is called in all GTK applications. Arguments are parsed
    * from the command line and are returned, to the application. */
   gtk_init (&argc, &argv);
   /* create a new window */
   window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
   gtk_window_set_default_size(GTK_WINDOW(window), 700, 400);
   /* When the window is given the "delete_event" signal (this is given
    * by the window manager, usually by the "close" option, or on the
    * titlebar), we ask it to call the delete_event () function
    * as defined above. The data passed to the callback
    * function is NULL and is ignored in the callback function. */
   g_signal_connect (G_OBJECT (window), "delete_event",
         G_CALLBACK (delete_event), NULL);
   /* Here we connect the "destroy" event to a signal handler.
    * This event occurs when we call gtk_widget_destroy() on the window,
    * or if we return FALSE in the "delete_event" callback. */
   g_signal_connect (G_OBJECT (window), "destroy",
         G_CALLBACK (destroy), NULL);
   /* Sets the border width of the window. */
   gtk_container_set_border_width (GTK_CONTAINER (window), 5);
   verticalBox= gtk_vbox_new (FALSE, 5);
   gtk_container_add (GTK_CONTAINER (window), verticalBox);
   buttonBox= gtk_hbox_new (FALSE, 10);
   //gtk_container_add (GTK_CONTAINER (verticalBox), buttonBox);
   gtk_box_pack_start (GTK_BOX(verticalBox), buttonBox, FALSE, FALSE, 5);
   button1= gtk_button_new_with_label ("Next step");
   button2= gtk_button_new_with_label ("Simulate all");
   button3= gtk_button_new_with_label ("Restart");
   g_signal_connect (button1, "clicked", G_CALLBACK (callbackNextStep), (gpointer) NULL);
   g_signal_connect (button2, "clicked", G_CALLBACK (callbackSimulateAll), (gpointer) NULL);
   g_signal_connect (button3, "clicked", G_CALLBACK (callbackRestart), (gpointer) NULL);
   //gtk_container_add (GTK_CONTAINER (buttonBox), button1);
   //gtk_container_add (GTK_CONTAINER (buttonBox), button2);
   gtk_box_pack_start (GTK_BOX(buttonBox), button1, FALSE, FALSE, 5);
   gtk_box_pack_start (GTK_BOX(buttonBox), button2, FALSE, FALSE, 5);
   gtk_box_pack_start (GTK_BOX(buttonBox), button3, FALSE, FALSE, 5);
#if DEBUG
   button4= gtk_button_new_with_label ("Go Back");
   g_signal_connect (button4, "clicked", G_CALLBACK (callbackGoBack), (gpointer) NULL);
   gtk_box_pack_start (GTK_BOX(buttonBox), button4, FALSE, FALSE, 5);
   button5= gtk_button_new_with_label ("Test");
   g_signal_connect (button5, "clicked", G_CALLBACK (callbackTest), (gpointer) NULL);
   gtk_box_pack_start (GTK_BOX(buttonBox), button5, FALSE, FALSE, 5);
#endif
   ///creo la tabla para los paneles
   table = gtk_table_new (2, 2+numberCaches, TRUE);
   /* Coloca la tabla en la ventana principal */
   //gtk_container_add (GTK_CONTAINER (verticalBox), table);
   gtk_box_pack_start (GTK_BOX(verticalBox), table, TRUE, TRUE, 5);
   gtk_table_set_row_spacings(GTK_TABLE(table), 10);
   gtk_table_set_col_spacings(GTK_TABLE(table), 10);
   ///creo los buttones
   /* Creo la columna que representa a la memory */
   createPanelMemory();
   for(int i=0; i<numberCaches; i++) {
      createPanelCache(i);
   }
   vboxText  = gtk_vbox_new(FALSE, 2);
   //creo el text view
   //buffer = gtk_text_buffer_new (NULL);
   text_view = gtk_text_view_new_with_buffer (buffer);
   gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_NONE);

   //In the initial state first line is going to be executed first so it must be coloured in blue.
   //iter at the next line
   GtkTextIter lineIterNext;
   gtk_text_buffer_get_iter_at_offset (buffer, &lineIterNext, 0);
   //iter at the line next to the next line
   GtkTextIter lineIterNextNext=lineIterNext;
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterNextNext);
   //coloreo de azul la linea siguiente
   gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterNext, &lineIterNextNext);

   scrolled_window = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
   //insertTextInPanel("holamundo");
   //creo su tag
   char cadena[200];
   sprintf(cadena, "TRACE FILE: %s", cpu.trace_file);
   GtkWidget *label;
   label =gtk_label_new (cadena);
   create_view_statistics();
   vbox_statistics  = gtk_vbox_new(FALSE, 2);
   //creo el text view
   //buffer = gtk_text_buffer_new (NULL);
   //text_view = gtk_text_view_new_with_buffer (buffer);
   //gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_NONE);
   scrolled_window2 = gtk_scrolled_window_new (NULL, NULL);
   gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_container_add(GTK_CONTAINER(scrolled_window2), estatistics_view);
   //insertTextInPanel("holamundo");
   //creo su tag
   sprintf(cadena, "Simulation statistics");
   GtkWidget *label2;
   label2 =gtk_label_new (cadena);
   gtk_box_pack_start(GTK_BOX(vboxText), label, FALSE, FALSE, 1);
   gtk_box_pack_start(GTK_BOX(vboxText), scrolled_window, TRUE, TRUE, 1);
   gtk_box_pack_start(GTK_BOX(vbox_statistics), label2, FALSE, FALSE, 1);
   gtk_box_pack_start(GTK_BOX(vbox_statistics), scrolled_window2, TRUE, TRUE, 1);
   //gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
   //gtk_container_add (GTK_CONTAINER (scrolled_window), scrolled_window);
   // Coloco los buttones en la tabla
   //printf("%d\n", numberCaches);
   gtk_table_attach_defaults (GTK_TABLE (table), vboxText, 0, 1, 0, 1);
   gtk_table_attach_defaults (GTK_TABLE (table), vbox_statistics, 0, 1, 1, 2);
   gtk_table_attach_defaults (GTK_TABLE (table), vboxMEMORY, numberCaches+1, numberCaches+2, 0, 2);
   for(int i=0; i<numberCaches; i++) {
      if(!caches[i].separated) {
         gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxData, 1+i, 2+i, 0, 2);
      } else {
         gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxData, 1+i, 2+i, 0, 1);
         gtk_table_attach_defaults (GTK_TABLE (table), cacheLevelPanels[i].vboxInstruction, 1+i, 2+i, 1, 2);
      }
   }
   //gtk_table_attach_defaults (GTK_TABLE (table), button2, 1, 2, 0, 2);
   //gtk_table_attach_defaults (GTK_TABLE (table), button3, 2, 3, 0, 2);
   //insertTextInPanel("holamundo");
   /* and the window */
   gtk_widget_show_all(window);
   /* All GTK applications must have a gtk_main(). Control ends here
    * and waits for an event to occur (like a key press or
    * mouse event). */
   gtk_main();
   return 1;
}

/*
 *  Get the next line from the trace file stored in the text widget without going to the next line.
 *  
 */
char *getCurrentLineTrace() {
   //iter at the current line
   GtkTextIter lineIterCurrent;
   gtk_text_buffer_get_iter_at_mark (buffer, &lineIterCurrent, marcaLineCurrent);
   //iter at the next line
   GtkTextIter lineIterNext=lineIterCurrent;
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterNext);
   char *currentLine = gtk_text_buffer_get_text (buffer, &lineIterCurrent, &lineIterNext, 1);
   if(currentLine!=NULL&&currentLine[0]=='\0'){
       return NULL;
   }

   return currentLine;
}




/*
 *  Go to the next line from the trace file stored in the text widget and get it.
 *  
 */
char *goToNextLineTrace() {
   //iter at the current line
   GtkTextIter lineIterCurrent;
   gtk_text_buffer_get_iter_at_mark (buffer, &lineIterCurrent, marcaLineCurrent);
   //iter at the first line.
   GtkTextIter start;
   gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
   //iter at the next line
   GtkTextIter lineIterNext=lineIterCurrent;
   int endOfTrace=gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterNext);

   //iter at the line next to the next line
   GtkTextIter lineIterNextNext=lineIterNext;
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterNextNext);
   

   GtkTextIter end=lineIterNext;
   //coloreo de negro todas las lineas anteriores y la actual
   gtk_text_buffer_remove_all_tags (buffer,
        &start,
        &end);


   //coloreo de azul la linea actual
   //gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterCurrent, &lineIterSiguiente);

   if(endOfTrace==0){
       //printf("se acabó\n");
       return NULL;
      
   }

   //coloreo de azul la linea siguiente
   gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterNext, &lineIterNextNext);

   //scroll until new line is visible
   gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(text_view), &lineIterNext, 0.0, 0, 0.0, 0.0);
   //Store the current line
   char *currentLine = gtk_text_buffer_get_text (buffer, &lineIterNext, &lineIterNextNext, 1);
   
   //printf("%s", lineCurrent);
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterCurrent);
   //printf("%d\n", endOfTrace);


   gtk_text_buffer_move_mark (buffer, marcaLineCurrent, &lineIterCurrent);

   //printf("%s", currentLine); 
   //printf("aaaaaaa\n"); 
   return currentLine;
}
 
char *goToPreviousLineTrace() {

   //iter at the current line
   GtkTextIter lineIterCurrent;
   gtk_text_buffer_get_iter_at_mark (buffer, &lineIterCurrent, marcaLineCurrent);
   //iter at the first line.
   GtkTextIter start;
   gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);

   //iter at the previous line
   GtkTextIter lineIterPrevious=lineIterCurrent;

   int principio=gtk_text_view_backward_display_line (GTK_TEXT_VIEW(text_view), &lineIterPrevious);
   if(principio==0){
       //printf("se acabó\n");
       return NULL;
   }
   principio=gtk_text_view_backward_display_line (GTK_TEXT_VIEW(text_view), &lineIterPrevious);
   if(!principio){
       gtk_text_buffer_get_iter_at_offset (buffer, &lineIterPrevious, 0);
       //return "dasdasd";
   }else{
       gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterPrevious);
   }


   //iter at the next line
   GtkTextIter lineIterNext=lineIterCurrent;
   gtk_text_view_forward_display_line (GTK_TEXT_VIEW(text_view), &lineIterNext);

   //gtk_text_view_backward_display_line_start (GTK_TEXT_VIEW(text_view), &lineIterPrevious);


   //GtkTextIter end=lineIterNext;

   // Remove all tags for removing colour.
   gtk_text_buffer_remove_all_tags (buffer,
        &start,
        &lineIterNext);
   //end=lineIterCurrent;

   //coloreo de negro todas las lineas anteriores y la actual
   //gtk_text_buffer_apply_tag (buffer, tagBlack, &start, &lineIterPrevious);

   //coloreo de azul la linea actual
   //gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterCurrent, &lineIterNext);

   //coloreo de azul la linea anterior
   gtk_text_buffer_apply_tag (buffer, tagBlue, &lineIterPrevious, &lineIterCurrent);

   //scroll until new line is visible
   gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(text_view), &lineIterPrevious, 0.0, 0.0, 0.0, 0.0);
   //Store the current line
   char *currentLine = gtk_text_buffer_get_text (buffer, &lineIterPrevious, &lineIterCurrent, 1);
   
   //printf("%s", lineCurrent);
   gtk_text_buffer_move_mark (buffer, marcaLineCurrent, &lineIterPrevious);
   //gtk_text_buffer_add_mark (buffer, marcaLineCurrent, &lineIterCurrent);

   
   return currentLine;

}



void address_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
   guint  number;
   gchar  buf[64];
   gtk_tree_model_get(model, iter, user_data, &number, -1);
   g_snprintf(buf, sizeof(buf), "%0*x", (int)cpu.address_width/4, number);
   g_object_set(renderer, "text", buf, NULL);
}

void content_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
   guint  number;
   gchar  buf[64];
   gtk_tree_model_get(model, iter, user_data, &number, -1);
   g_snprintf(buf, sizeof(buf), "%0*x", (int)cpu.word_width/4, number);
   g_object_set(renderer, "text", buf, NULL);
}

/*
 * Funtion to create the Memory Panel for the GUI. Memory data structure must have been created previously.
 */
GtkWidget * createPanelMemory() {
   GtkTreeIter   iter;
   GtkTreeViewColumn* column;
   GtkCellRenderer *renderer;
   vboxMEMORY = gtk_vbox_new(FALSE, 2);
   viewMEMORY = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelMEMORY));
   GtkTreeSelection * selection= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewMEMORY));
   gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);
   renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes("Address",
         renderer,
         "text", ADDRESS,
         "foreground", COLOR,
         NULL);
   gtk_tree_view_append_column(GTK_TREE_VIEW(viewMEMORY), column);
   gtk_tree_view_column_set_cell_data_func(column, renderer, address_cell_data_func, (void*) ADDRESS, NULL);

   renderer = gtk_cell_renderer_text_new();
   column = gtk_tree_view_column_new_with_attributes("Content",
         renderer,
         "text", CONTENT,
         "foreground", COLOR,
         NULL);
   gtk_tree_view_append_column(GTK_TREE_VIEW(viewMEMORY), column);
   gtk_tree_view_column_set_cell_data_func(column, renderer, content_cell_data_func, (void*) CONTENT, NULL);
   GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
   gtk_container_add(GTK_CONTAINER(scwin), viewMEMORY);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
         GTK_POLICY_ALWAYS);
   GtkWidget *label;
   label =gtk_label_new ("MEMORY");
   gtk_box_pack_start(GTK_BOX(vboxMEMORY), label, FALSE, FALSE, 1);
   gtk_box_pack_start(GTK_BOX(vboxMEMORY), scwin, TRUE, TRUE, 1);
   return vboxMEMORY;
}

void line_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
   int level = (int) user_data;
   guint  number;
   gchar  buf[64];
   gtk_tree_model_get(model, iter, LINE, &number, -1);
   g_snprintf(buf, sizeof(buf), "%0*x", caches[level].hexDigsLine, number);
   g_object_set(renderer, "text", buf, NULL);
}

void set_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
   int level = (int) user_data;
   guint  number;
   gchar  buf[64];
   gtk_tree_model_get(model, iter, SET, &number, -1);
   g_snprintf(buf, sizeof(buf), "%0*x", caches[level].hexDigsSet, number);
   g_object_set(renderer, "text", buf, NULL);
}

void tag_cell_data_func (GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
   int level = (int) user_data;
   guint  number;
   gchar  buf[64];
   gtk_tree_model_get(model, iter, TAG, &number, -1);
   g_snprintf(buf, sizeof(buf), "%0*x", caches[level].hexDigsTag, number);
   g_object_set(renderer, "text", buf, NULL);
}

/**
 * Function to create the GUI panels of a cache level. Cache data structure must have been created previously.
 * @param level whose GUI panel will be generated.
 *
 */
void createPanelCache(int level) {
   //char* mask="11000011";
   const char* mask= caches[level].column_bit_mask;
   //Creo la cache. en caso de que sea dividia esta será la parte de data
   GtkTreeIter   iter;
   GtkTreeViewColumn* column;
   GtkCellRenderer *renderer;
   GtkListStore *modelData;
   GtkWidget *vboxData;
   GtkWidget *viewData;
   vboxData = gtk_vbox_new(FALSE, 2);
   //createCacheModel(level);
   modelData=cacheLevels[level].modelData;
   viewData = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelData));
   GtkTreeSelection * selectionData= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewData));
   gtk_tree_selection_set_mode (selectionData, GTK_SELECTION_NONE);
   //CREO LAS COLUMNAS A MOSTRAR EN LA TABLA. No confundir con las columnas de la estructura de dados model
   if(mask[LINE]=='1'){
      renderer = gtk_cell_renderer_text_new();
      column = gtk_tree_view_column_new_with_attributes("Line",
            renderer,
            "text", LINE,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
      gtk_tree_view_column_set_cell_data_func(column, renderer, line_cell_data_func, (void*) level, NULL);
   }
   if(mask[SET]=='1'){
      renderer = gtk_cell_renderer_text_new();
      column = gtk_tree_view_column_new_with_attributes("Set",
            renderer,
            "text", SET,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
      gtk_tree_view_column_set_cell_data_func(column, renderer, set_cell_data_func, (void*) level, NULL);
   }
   if(mask[VALID]=='1'){
      renderer = gtk_cell_renderer_text_new();
      column = gtk_tree_view_column_new_with_attributes("V",
            renderer,
            "text", VALID,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   if(mask[TAG]=='1'){
      renderer = gtk_cell_renderer_text_new();
      column = gtk_tree_view_column_new_with_attributes("Tag",
            renderer,
            "text", TAG,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
      gtk_tree_view_column_set_cell_data_func(column, renderer, tag_cell_data_func, (void*) level, NULL);
   }
   if(mask[DIRTY]=='1'){
      column = gtk_tree_view_column_new_with_attributes("D",
            gtk_cell_renderer_text_new(),
            "text", DIRTY,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   if(mask[TIMES_ACCESSED]=='1'){
      column = gtk_tree_view_column_new_with_attributes("AC",
            gtk_cell_renderer_text_new(),
            "text", TIMES_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   if(mask[LAST_ACCESSED]=='1'){
      column = gtk_tree_view_column_new_with_attributes("LA",
            gtk_cell_renderer_text_new(),
            "text", LAST_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   if(mask[FIRST_ACCESSED]=='1'){
      column = gtk_tree_view_column_new_with_attributes("FA",
            gtk_cell_renderer_text_new(),
            "text", FIRST_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   if(mask[CONTENT_CACHE]=='1'){
      column = gtk_tree_view_column_new_with_attributes("Content",
            gtk_cell_renderer_text_new(),
            "text", CONTENT_CACHE,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewData), column);
   }
   ////////////////////////////////////////////////////////
   GtkWidget *scwin = gtk_scrolled_window_new(NULL, NULL);
   gtk_container_add(GTK_CONTAINER(scwin), viewData);
   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,
         GTK_POLICY_ALWAYS);
   GtkWidget *label;
   //si no es una cache dividida pongo el titulo y return de la funcion
   if(!caches[level].separated) {
      char cadena[50];
      sprintf(cadena, "L%d CACHE", level+1);
      label =gtk_label_new (cadena);
      gtk_box_pack_start(GTK_BOX(vboxData), label, FALSE, FALSE, 1);
      gtk_box_pack_start(GTK_BOX(vboxData), scwin, TRUE, TRUE, 1);
      cacheLevels[level].modelData=modelData;
      cacheLevelPanels[level].vboxData=vboxData;
      cacheLevelPanels[level].viewData=viewData;
      return;
      //si es dividida lo titulo como cache de data y creo la parte correspondiente a instructions
   } else {
      char cadena[50];
      sprintf(cadena, "L%d DATA CACHE", level+1);
      label =gtk_label_new (cadena);
      gtk_box_pack_start(GTK_BOX(vboxData), label, FALSE, FALSE, 1);
      gtk_box_pack_start(GTK_BOX(vboxData), scwin, TRUE, TRUE, 1);
      cacheLevels[level].modelData=modelData;
      cacheLevelPanels[level].vboxData=vboxData;
      cacheLevelPanels[level].viewData=viewData;
      GtkTreeIter   iterInstruction;
      GtkTreeViewColumn* columnInstruction;
      GtkListStore *modelInstruction;
      GtkWidget *vboxInstruction;
      GtkWidget *viewInstruction;
      vboxInstruction = gtk_vbox_new(FALSE, 2);
      modelInstruction=cacheLevels[level].modelInstruction;
      viewInstruction = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modelInstruction));
      GtkTreeSelection * selectionInstruction= gtk_tree_view_get_selection (GTK_TREE_VIEW(viewInstruction));
      gtk_tree_selection_set_mode (selectionInstruction, GTK_SELECTION_NONE);
      //CREO LAS COLUMNAS A MOSTRAR EN LA TABLA. No confundir con las columnas de la estructura de dados model
   if(mask[LINE]=='1'){
      renderer = gtk_cell_renderer_text_new();
      columnInstruction = gtk_tree_view_column_new_with_attributes("Line",
            renderer,
            "text", LINE,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
      gtk_tree_view_column_set_cell_data_func(columnInstruction, renderer, line_cell_data_func, (void*) level, NULL);
   }
   if(mask[SET]=='1'){
      renderer = gtk_cell_renderer_text_new();
      columnInstruction = gtk_tree_view_column_new_with_attributes("Set",
            renderer,
            "text", SET,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
      gtk_tree_view_column_set_cell_data_func(columnInstruction, renderer, set_cell_data_func, (void*) level, NULL);
   }
   if(mask[VALID]=='1'){
      renderer = gtk_cell_renderer_text_new();
      columnInstruction = gtk_tree_view_column_new_with_attributes("V",
            renderer,
            "text", VALID,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
   if(mask[TAG]=='1'){
      renderer = gtk_cell_renderer_text_new();
      columnInstruction = gtk_tree_view_column_new_with_attributes("Tag",
            renderer,
            "text", TAG,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
      gtk_tree_view_column_set_cell_data_func(columnInstruction, renderer, tag_cell_data_func, (void*) level, NULL);
   }
   if(mask[DIRTY]=='1'){
      columnInstruction = gtk_tree_view_column_new_with_attributes("D",
            gtk_cell_renderer_text_new(),
            "text", DIRTY,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
   if(mask[TIMES_ACCESSED]=='1'){
      columnInstruction = gtk_tree_view_column_new_with_attributes("AC",
            gtk_cell_renderer_text_new(),
            "text", TIMES_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
   if(mask[LAST_ACCESSED]=='1'){
      columnInstruction = gtk_tree_view_column_new_with_attributes("LA",
            gtk_cell_renderer_text_new(),
            "text", LAST_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
   if(mask[FIRST_ACCESSED]=='1'){
      columnInstruction = gtk_tree_view_column_new_with_attributes("FA",
            gtk_cell_renderer_text_new(),
            "text", FIRST_ACCESSED,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
   if(mask[CONTENT_CACHE]=='1'){
      columnInstruction = gtk_tree_view_column_new_with_attributes("Content",
            gtk_cell_renderer_text_new(),
            "text", CONTENT_CACHE,
            "foreground", COLOR_CACHE,
            NULL);
      gtk_tree_view_append_column(GTK_TREE_VIEW(viewInstruction), columnInstruction);
   }
      ///////////////////////////////////////////////////
      GtkWidget *scwinInstruction = gtk_scrolled_window_new(NULL, NULL);
      gtk_container_add(GTK_CONTAINER(scwinInstruction), viewInstruction);
      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwinInstruction), GTK_POLICY_AUTOMATIC,
            GTK_POLICY_ALWAYS);
      GtkWidget *labelInstruction;
      char cadenaInstructions[50];
      sprintf(cadenaInstructions, "L%d INST. CACHE", level+1);
      labelInstruction =gtk_label_new (cadenaInstructions);
      gtk_box_pack_start(GTK_BOX(vboxInstruction), labelInstruction, FALSE, FALSE, 1);
      gtk_box_pack_start(GTK_BOX(vboxInstruction), scwinInstruction, TRUE, TRUE, 1);
      cacheLevels[level].modelInstruction=modelInstruction;
      cacheLevelPanels[level].vboxInstruction=vboxInstruction;
      cacheLevelPanels[level].viewInstruction=viewInstruction;
      return;
   }
}

/**
 * Function to create statistic panel in the gui. The statistics data structure must ghave been created previously.
 */
GtkWidget *create_view_statistics(void) {
   GtkTreeViewColumn   *col;
   GtkCellRenderer     *renderer;
   GtkTreeModel *model= statistics_model;
   GtkWidget *view=     estatistics_view;
   view = gtk_tree_view_new();
   /* --- Column #1 --- */
   col = gtk_tree_view_column_new();
   gtk_tree_view_column_set_title(col, "First Name");
   /* pack tree view column into tree view */
   gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
   renderer = gtk_cell_renderer_text_new();
   /* pack cell renderer into tree view column */
   gtk_tree_view_column_pack_start(col, renderer, TRUE);
   /* connect 'text' property of the cell renderer to
    *  model column that contains the first name */
   gtk_tree_view_column_add_attribute(col, renderer, "text", COMPONET_OR_PROPERTY);
   /* --- Column #2 --- */
   col = gtk_tree_view_column_new();
   gtk_tree_view_column_set_title(col, "Last Name");
   /* pack tree view column into tree view */
   gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
   renderer = gtk_cell_renderer_text_new();
   /* pack cell renderer into tree view column */
   gtk_tree_view_column_pack_start(col, renderer, TRUE);
   /* connect 'text' property of the cell renderer to
    *  model column that contains the last name */
   gtk_tree_view_column_add_attribute(col, renderer, "text", VALUE);
   /* set 'weight' property of the cell renderer to
    *  bold print (we want all last names in bold) */
   g_object_set(renderer,
         "weight", PANGO_WEIGHT_BOLD,
         "weight-set", TRUE,
         NULL);
   gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
   gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(view),0);
   g_object_unref(model); /* destroy model automatically with view */
   gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)),
         GTK_SELECTION_NONE);
   estatistics_view= view;
   return view;
}

/**
 * This function inserts text into the panel (actually it is being inserted into the buffer).
 * @param text to be added
 */
void insertTextInPanel(char* text) {
   GtkTextBuffer *buffer;
   GtkTextMark *mark;
   GtkTextIter iter;
   buffer =gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
   mark = gtk_text_buffer_get_insert (buffer);
   gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
   /* Insert newline (only if there's already text in the buffer). */
   //if (gtk_text_buffer_get_char_count(buffer))
   //gtk_text_buffer_insert (buffer, &iter, "\n", 1);
   gtk_text_buffer_insert (buffer, &iter, text, -1);
}

/**
 * Function for scrolling the panel to a text line
 * @param line which the panel will be scrolled to.
 */
void scrollTextViewToLine(long line) {
   GtkTextIter iter;
   gtk_text_buffer_get_iter_at_line (buffer,
         &iter,
         line);
   //scroll hasta que la nueva line es visible
   gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(text_view), &iter, 0.0, 0, 0.0, 0.0);
   return;
}

/**
 * Function for scrolling the memory panel to an address
 * @param address which the memory panel will be scrolled to.
 */
int scrollMemoryToPos(long address) {
   char rowChar[50];
   //if not word address return error
   if(address%(cpu.word_width/8)!=0){
      return -1;
   }
   //if out of page return error
   if(address<memory.page_base_address || address>(memory.page_base_address+memory.page_size)){
      return -2;
   }
   //get the table row from the memory address
   long i=(address-memory.page_base_address)/(cpu.word_width/8);  
   sprintf(rowChar, "%ld", i);
   GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
   gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(viewMEMORY),
         path, NULL, TRUE, 0.5, 0);
   return 0;
}

/**
 * Function for scrolling the cache panel to a row
 * @param level which will be scrolled.
 * @param row cache line which the cache panel will be scrolled to.
 */
void scrollCacheToRow(int level, long row) {
   scrollDataCacheToRow(level, row);
}

/**
 * Function for scrolling the data cache panel to a row
 * @param level which will be scrolled.
 * @param row cache line which the data cache panel will be scrolled to.
 */
void scrollDataCacheToRow(int level, long row) {
   char rowChar[50];
   sprintf(rowChar, "%ld", row);
   GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
   gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(cacheLevelPanels[level].viewData),
         path, NULL, TRUE, 0.5, 0);
}

/**
 * Function for scrolling the instruction cache panel to a row
 * @param level which will be scrolled.
 * @param row cache line which the instruction cache panel will be scrolled to.
 */
void scrollInstructionCacheToRow(int level, long row) {
   char rowChar[50];
   sprintf(rowChar, "%ld", row);
   GtkTreePath * path=gtk_tree_path_new_from_string (rowChar);
   gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(cacheLevelPanels[level].viewInstruction),
         path, NULL, TRUE, 0.5, 0);
}

/**
 * Function for printing error messages. If gui mode it shows error dialog.
 * @param message to print
 */
void printErrorMessage(char * message, int lineNumber){

   if(window==NULL){
       fprintf(stderr, "%s Line %d\n", message, lineNumber);
   }else{
        GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                  GTK_DIALOG_DESTROY_WITH_PARENT,
                                  GTK_MESSAGE_ERROR,
                                  GTK_BUTTONS_CLOSE,
                                  "%s", message);
        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);
   }
}
