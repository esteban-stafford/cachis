#ifndef GUI_H
#define GUI_H

#include "computer.h"

// Dimensions and spacing
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define MARGIN_MED 10
#define MARGIN_SMALL 5
#define CACHE_WIDGET_MIN_WIDTH 300	// This makes middle column more prone to overflow as cache widgets get wider

// Colors
#define HIGHLIGHT_COLOR "#219ebc"

// Text fields
#define WINDOW_NAME "Cache Simulation"
#define TXT_TRACE "Trace file:"
#define TXT_SIMSTATS "Simulation statistics:"

#define C_SEP_DATA "Data"
#define C_SEP_INST "Instructions"
#define C_LINE "Line"
#define C_SET "Set"
#define C_VALID "V"
#define C_DIRTY "D"
#define C_ACC "Accesses"
#define C_LASTACC "Last Acc."
#define C_FIRSTACC "First Acc."
#define C_TAG "Tag"
#define C_CONTENT "Content"

#define M_ADDR "Address"
#define M_CONT "Content"


static GtkApplication *app = NULL;
extern int useGUI;

int launch_gui(int argc, char **argv, Computer *computer);
void print_error_message(const char * message, int lineNumber);

#endif
