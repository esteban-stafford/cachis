#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>

#include "dictionary.h"
#include "confparser.h"
#include "computer.h"
#include "traceparser.h"
#include "datainterface.h"
#include "datastore.h"
#include "gui.h"
#include "simulator.h"
#include "statistics.h"

#define PROGRAM_NAME "cachis"
#define VERSION "0.1"

#define DEFAULT_FILENAME "cachis.stl"

int useGUI = 1;
char *ini_file = NULL;
char *dramsys_file = NULL;
int generate_dramsys_trace = 0;


void printUsage() {
   printf(
      "Usage: %s [OPTION]... <file>.ini [<file>.vca]\n"
      "Simulate the memory hierarchy defined in <file>.ini.\n"
      "Optionally provide a trace file. This overides the one specified in the ini file.\n"
      "This a list of the options accepted:\n"
      "\n"
	  "  -i <file> Indicates which .ini file should be loaded.\n"
	  "  -e <file> Export a DRAMSys memory trace file to the specified file\n"
      "  -g        Toggle GUI\n"
      "  -h        Display this help and exit\n"
      "  -v        Output version information and exit\n"

	  "\n"
      "Exit status:\n"
      " 0  if OK,\n"
      " 1  if configuration or other kind of errors.\n"
      "\n", PROGRAM_NAME);
}

int main(int argc, char *argv[]) {
   int arg;

   // The random seed gets initiated to a default value
   srand(time(NULL));

   // getopt won't print any errors, the user must take care of covering them in ? or :
   opterr = 0;

   // Parse command line arguments
   while ((arg = getopt (argc, argv, "ghvi:e:")) != -1)
    switch (arg)
      {
	  case 'i':
		// The ini file is stored
		ini_file = optarg;

		// If the next argument has been read as a file name because no filename has been provided, abort
		if (ini_file[0] == '-') {
			fprintf (stderr, "Flag -i requires a filename.\n");
			return 1;
		}
		break;

	  case 'e':
		generate_dramsys_trace = 1;
		dramsys_file = optarg;

		// If the next argument has been read as a file name because no filename has been provided, abort
		if (dramsys_file[0] == '-') {
			fprintf (stderr, "Flag -e requires a filename.\n");
			return 1;
		}

		// If the file has not been specified, it gets set to DEFAULT_FILENAME
		if (dramsys_file == NULL) {
			dramsys_file = DEFAULT_FILENAME;
			printf("No filename has been specified, using the default %s\n", dramsys_file);
		} else {
			printf("Saving trace to %s\n", dramsys_file);
		}

		// The file gets created, if it exists, it will be truncated
		FILE *file = fopen(dramsys_file, "w");

		if (file == NULL) {
			printf("Error opening the file.\n");
			return 1;
		}

		fclose(file);
        break;
      case 'g':
         useGUI = 0;
        break;
      case 'h':
        printUsage();
        return 0;
      case 'v':
        printf("%s version %s\n",PROGRAM_NAME,VERSION);
        return 0;
	  case ':':
		  fprintf(stderr, "Flag -%c requires a filename", arg);
		  return 1;
	  case '?':
        if (optopt == 'e' || optopt == 'i')
			fprintf (stderr, "Flag -%c requires a filename.\n", optopt);
        return 1;
      default:
        abort();
      }

    // Read configuration file
    dictionary *ini;

    if((ini = readConfigurationFile(ini_file)) == NULL) {
       return 1;
    }

    Computer computer;
    // Parse read configuration and populate global configuration variables.
    if(parseConfiguration(ini, &computer) != 0) {
       return 1;
    }

    // Create simulator data structures
    generateDataStorage(&computer);

    // Override trace file if there is a filename on the command line.
    if(optind+1 < argc) {
       computer.cpu.trace_file = argv[optind +1];
    }

    // Load trace file specified in the configuration file
    if(readTraceFile(&computer) != 0) {
       return 1;
    }

    // Start GUI...
    if(useGUI) {
       printf("Operations to be simulated: %d\n", numberOfOperations);
       launch_gui(1, argv, &computer);
    // ... or run batch simulation
    } else {
       printf("Running Cachis in CLI mode\n");
       printf("Starting simulation:\n");
       printf("Operations to be simulated: %d\n", numberOfOperations);
       simulate(&computer);
    }


    // Free memory allocated by readTraceFile()
    freeMemory();

    return 0;
}
