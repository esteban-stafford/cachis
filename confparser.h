#ifndef CONFPARSER_H
#define CONFPARSER_H

#include "iniparser.h"

  struct structCpu
  {
      long address_width;
      long word_width;            // Número de bits en la word del procesador
      long frequency;             // Frecuencia en herzios. Ver Nota 1.
      long bus_frequency;         // Frecuencia en herzios del bus longerno. Ver Nota 1.
      long mem_bus_frequency;     // Frecuencia en herzios del bus. Ver Nota 1.
      const char* trace_file;     // File de trace
  };

  struct structMemory
  {
      long size;                  // Tamaño de la memory en bytes. Ver Nota 2.
      long bus_width;             // Tamaño del bus extremo en bits.
      long bus_frequency;         // Frecuencia en herzios del bus. Ver Nota 1.
      double access_time_1;       // Timepo de acceso en nanosegundos.
      double access_time_burst;
      long page_size;
      long page_base_address;
  };

  struct structCache
  {
      long line_size;               // Tamaño de line en bytes. Ver Nota 2, aunque no has mucho sentido prácitco, pero seamos coherentes.
      long size;                    // Tamaño cache en bytes. Ver Nota 2.
      long asociativity;            // Asociatividad: 1= Mapeo directo, F= totalmente asociatva, cualquier user_use número potencia de dos.    
      long write_policy;            // Política de escritura: wt = write through, wb = write back
      long replacement;             // Política de reemplazo: lru, rand
      int separated;                // Si la memory cache es separada. Nota 3.
      double access_time;
      const char* column_bit_mask;  //Esto indica que columnas se muestran y cuales se ocultan. Mascara en binario

      int numLines;
      int numSets;
      int numWords;
      int hexDigsSet;
      int hexDigsLine;
      int hexDigsTag;
      int offsetBits;
      int setBits;
  };

  #define NCLAVES_CPU 4
  char* keysCPU[NCLAVES_CPU];
  #define NCLAVES_MEMORY 5
  char* keysMEMORY[NCLAVES_MEMORY];
  #define NCLAVES_CACHE 8
  char* keysCACHE[NCLAVES_CACHE];

  #define MAX_CACHES 10
  int numberCaches;

  #define N_BITS_MASK 9

  struct structCpu cpu;
  struct structMemory memory;
  struct structCache caches[MAX_CACHES];

  dictionary *readConfigurationFile(char * file);
  int parseConfiguration(dictionary *ini);
  void checkSectionKeys(dictionary *ini, const char *section, int numberOfValidKeys, char *validKeys[], int *errors);
  void showConfiguration();

#endif
