# Code Documentation
## include
### computer.h

computer.h contains all the structures for the CPU, Memory, Generic Cache and defines a computer with all 3 elements. The computer can have up to 10 levels of cache.

### confparser.h

It contains all the function declarations for the confparser.c file and a mask constant.


### datainterface.h

Contains an enum with colors, structs for a cache line with a line, tag, valid and dirty bits and statistics for access as well as a memory position with address and content.

All the following functions stablish operations for memory/cache management such as resetting both, showing lines, finding tags, reading lines, flags, updating and statistics  ...

### datamanipulation.h

Contains enums for the replacement and write policies as well as basic functions to parse and manipulate data.

### datastore.h

### dictionary.h

Part of the iniparser library.

### gui.c

Some constants and function declarations for the GUI.

### simulator.h

Function declarations and the definition of the response time struct

### traceparser.h

Contains enums for loading/ storing instructions or data, some constants, the memOperation struct that defines the type of operation, address and if it's meant for instructions or data, and the data itself.


## src
### cachis.c

Reads the arguments for the configuration files and launches the application.

### confparser.c

Parses the configuration file with the iniparser library and populates all the computer values in the parseConfiguration function. It also provides some extra functions such as showConfiguration, which prints how a computer is set up.

### datainterface.c

Contains operations to reset memory and cache, read lines and flags.
Aside from memory operations, it also handles some GUI tasks that are currently commented.

### datamanipulation.c

Contains miscelaneous data operations such as parsing different data types, policies, converting string to arrays...

### datastore.c



### dictionary.c

Part of the iniparser library.

### gui.c

Handles all major gui operations and window definitions.

### simulator.c

Currently contains all the code required to simulate a step in the simulate_step function.

### traceparser.c

Primarily parses the trace file and offers related functions to the trace.
