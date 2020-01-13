#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "confparser.h"

extern int useGUI;

/* GtkWidget is the storage type for widgets */
   GtkWidget *window;

   GtkWidget *text_view;
   GtkWidget *scrolled_window;
   GtkWidget *scrolled_window2;
   GtkWidget *vboxText;
   GtkWidget *vbox_statistics;

   GtkWidget *table;
   GtkWidget *verticalBox;

   GtkWidget *button1;
   GtkWidget *button2;
   GtkWidget *button3;
#if DEBUG
   GtkWidget *button4;
   GtkWidget *button5;
#endif

   GtkWidget *buttonBox;
   GtkTextBuffer *buffer;

   GtkTextMark *marcaLineCurrent;

   GtkTextTag *tagBlue;
   GtkTextTag *tagBlack;

   GtkTextIter lineIterInicial;

   GtkTreeIter iter;

   struct cacheLevelPanel{
     GtkWidget *vboxData;//container de la tabla
     GtkWidget *viewData;//widget tabla
     GtkWidget *vboxInstruction;//container de la tabla
     GtkWidget *viewInstruction;//widget tabla
   };

   GtkWidget *vboxMEMORY;//container de la tabla
   GtkWidget *viewMEMORY;//widget tabla

   struct cacheLevelPanel cacheLevelPanels[MAX_CACHES];

   GtkWidget *estatistics_view;

   int generateGUI();
   GtkWidget * createPanelMemory();
   void createPanelCache(int level);
   GtkWidget *create_view_statistics(void);
   void insertTextInPanel(char* text);
   char *getCurrentLineTrace();
   char *goToNextLineTrace();
   char *goToPreviousLineTrace();
   int scrollMemoryToPos(long address);
   void scrollCacheToRow(int level, long row);
   void scrollDataCacheToRow(int level, long row);
   void scrollInstructionCacheToRow(int level, long row);
   void scrollTextViewToLine(long line);
   void printErrorMessage(char * message, int lineNumber);

#endif
