#ifndef COMPUTER_H
#define COMPUTER_H

#include <gtk/gtk.h>

typedef struct
{
    long address_width;
    long word_width;            // Número de bits en la word del procesador
    long frequency;             // Frecuencia en herzios. Ver Nota 1. TODO Deprecate this
    long bus_frequency;         // Frecuencia en herzios del bus longerno. Ver Nota 1. TODO
    long mem_bus_frequency;     // Frecuencia en herzios del bus. Ver Nota 1. TODO
    const char* trace_file;

    GtkTextBuffer *buffer;
    GtkTextView *view;
} Cpu;

typedef struct
{
    long size;                  // Size of the memory in Bytes
    long bus_width;             // Size of the bus in bits. TODO Deprecate this.
    long bus_frequency;         // Frequency of the bus in hertz. TODO
    double access_time_1;       // Access time in ns.
    double access_time_burst;   // Burst access time in ns. Used to calculate the time for the remaining words of an access after thw first one has been accessed.
    long page_size; 			// Size of the memory from the starting address
    long page_base_address;

    GListStore *model; 
    GtkWidget *view;
} Memory;

typedef struct
{
    long line_size;               // Size of a chace line in Bytes.
    long size;                    // Size of the cache in Bytes.
    long associativity;           // Associativity. 1 = Direct, F = Fully associative, power of 2 = N-way associative
    long write_policy;            // Write policy. WT, WB
    long replacement_policy;      // Replacement policy. LRU, LRU, RANDOM, FIFO
    int separated;                // If the cache is separated. 0 = False, 1 = True
    double access_time;
    const char* column_bit_mask;  // TODO Deprecate this

    int num_lines;
    int num_sets;
    int num_words;
    int hex_digs_set;       // TODO
    int hex_digs_line;      // TODO
    int hex_digs_tag;       // TODO
    int offset_bits;
    int set_bits;

    GListStore *model_data;
    GtkWidget *view_data;
    GListStore *model_instruction;
    GtkWidget *view_instruction;
} Cache;

#define MAX_CACHES 10
typedef struct
{
    Cpu cpu;
    Memory memory;
    Cache cache[MAX_CACHES];
    int num_caches;
} Computer;

#endif
