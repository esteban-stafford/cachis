#ifndef DATAMANIPULATION_H
#define DATAMANIPULATION_H

#include "confparser.h"
enum replacement {LRU=0, LFU=1, RANDOM=2, FIFO=3};
enum write_policy {WRITE_THROUGH=0, WRITE_BACK=1};

char* replacementPolicyStr(enum replacement replacement);
char* writePolicyStr(enum write_policy write_policy);
long parseLongK1000(const char * cadena);
long parseLongK1024(const char * cadena);
int parseBoolean(const char * cadena);
int parseInt(const char * cadena);
int parseReplacementPolicy(const char * cadena);
int parseWritePolicy(const char * cadena);
int isPowerOf2(long number);
int isAMultipleOf8(long number);
int isCorrectBinary(const char * cadena);
double parseDouble(const char * cadena);
long parseAddress(const char* page_base_address);
int isCorrectHexadecimal(char * number);
int isCorrectDecimal(char * number);
void contentArrayToString(unsigned* array, char* content, int count, int width);
void contentStringToArray(unsigned* array, char* content, int level);

#endif
