/**
 * @file traceparser.h
 * @brief Headers and constants related to traceparser.c
 */

#ifndef TRACEPARSER_H
#define TRACEPARSER_H


enum instructionOrData { DATA=1, INSTRUCTION=2 };
enum loadOrStore { LOAD=3, STORE=4 };

#define DEFAULT_SIZE 0;
#define DEFAULT_DATA 0;
#define DEFAULT_HAS_BREAK_POINT 0;
#define DEFAULT_INSTRUCTION_OR_DATA 0;
#define DEFAULT_OPERATION_TYPE 0;
#define DEFAULT_ADDRESS 0;

typedef struct {
  int hasBreakPoint;
  int instructionOrData;
  long address;
  int operation;
  int size;
  long data;
} MemoryOperation;

//array que almacena las operations de memory leidas del file de trace
extern MemoryOperation* memoryOperations;
extern int numberOfOperations;

int readTraceFile(Computer *computer);
void freeMemory();
void showOperations(Cpu *cpu);
int preprocessTraceLine(char *currentLine);
int parseLine(char* line, int lineNumber, MemoryOperation *result, int defaultSize, Memory *memory);
void printMemOperation(FILE *fp, MemoryOperation *operation, int cpu_address_width);
#endif
