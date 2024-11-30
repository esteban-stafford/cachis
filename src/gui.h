#ifndef GUI_H
#define GUI_H

#include "computer.h"

static GtkApplication *app = NULL;
extern int useGUI;

int launch_gui(int argc, char **argv, Computer *computer);
void print_error_message(const char * message, int lineNumber);

#endif
