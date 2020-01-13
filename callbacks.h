#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "gui.h"


void callbackSimulateAll( GtkWidget *widget, gpointer data);
void callbackNextStep( GtkWidget *widget, gpointer data);
void callbackRestart( GtkWidget *widget, gpointer data);
void destroy( GtkWidget *widget, gpointer data );
gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data );

#if DEBUG
void callbackGoBack( GtkWidget *widget, gpointer   data);
void callbackTest( GtkWidget *widget, gpointer   data);
#endif

#endif 
