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

struct memOperation {
  int hasBreakPoint;
  int instructionOrData;
  long address;
  int operation;
  int size;
  long data;
};

//array que almacena las operations de memory leidas del file de trace
extern struct memOperation* memoryOperations;
extern int numberOfOperations;

int readTraceFile(Computer *computer);
void freeMemory();
void showOperations(Cpu *cpu);
int preprocessTraceLine(char *currentLine);
int parseLine(char* line, int lineNumber, struct memOperation *result, int defaultSize, Memory *memory);
void printMemOperation(FILE *fp, struct memOperation *operation, int cpu_address_width);
#endif
