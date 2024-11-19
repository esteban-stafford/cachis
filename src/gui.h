#ifndef GUI_H
#define GUI_H

#include "computer.h"

static int useGUI = 1;
static GtkApplication *app = NULL;

int launch_gui(int argc, char **argv, Computer *computer);
void print_error_message(const char * message, int lineNumber);

#endif
