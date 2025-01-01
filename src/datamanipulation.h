#ifndef DATAMANIPULATION_H
#define DATAMANIPULATION_H

#include "confparser.h"
#include "replacementpolicy.h"
#include "writepolicy.h"

// Write policy functions
char* replacementPolicyStr(enum replacement replacement);
char* writePolicyStr(enum write_policy write_policy);

// Parse functions
long parseLongK1000(const char * cadena);
long parseLongK1024(const char * cadena);
int parseBoolean(const char * cadena);
int parseInt(const char * cadena);
int parseReplacementPolicy(const char * cadena);
int parseWritePolicy(const char * cadena);
double parseDouble(const char * cadena);
long parseAddress(const char* page_base_address);

// Checking functions
int isPowerOf2(long number);
int isAMultipleOf8(long number);
int isCorrectBinary(const char * cadena);
int isCorrectHexadecimal(char * number);
int isCorrectDecimal(char * number);

// Conversion functions
void contentArrayToString(unsigned* array, char* content, int count, int width);
void contentStringToArray(unsigned* array, char* content, int level);

// DRAMSys functions
int open_dramsys_file(const char *filename, FILE **f);
void write_to_dramsys_file(FILE **f, int lastNumber, int readOrWrite, int address);
void close_dramsys_file(FILE **f);

// Misc Functions
int cycle_rand();




#endif
