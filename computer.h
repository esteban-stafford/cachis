#ifndef COMPUTER_H
#define COMPUTER_H

#include <gtk/gtk.h>

typedef struct
{
    long address_width;
    long word_width;            // Número de bits en la word del procesador
    long frequency;             // Frecuencia en herzios. Ver Nota 1.
    long bus_frequency;         // Frecuencia en herzios del bus longerno. Ver Nota 1.
    long mem_bus_frequency;     // Frecuencia en herzios del bus. Ver Nota 1.
    const char* trace_file;     // File de trace

    GtkTextBuffer *buffer;
    GtkTextView *view;
} Cpu;

typedef struct
{
    long size;                  // Tamaño de la memory en bytes. Ver Nota 2.
    long bus_width;             // Tamaño del bus extremo en bits.
    long bus_frequency;         // Frecuencia en herzios del bus. Ver Nota 1.
    double access_time_1;       // Timepo de acceso en nanosegundos.
    double access_time_burst;
    long page_size;
    long page_base_address;

    GListStore *model; 
    GtkWidget *view;
} Memory;

typedef struct
{
    long line_size;               // Tamaño de line en bytes. Ver Nota 2, aunque no has mucho sentido prácitco, pero seamos coherentes.
    long size;                    // Tamaño cache en bytes. Ver Nota 2.
    long associativity;           // Asociatividad: 1= Mapeo directo, F= totalmente asociatva, cualquier user_use número potencia de dos.    
    long write_policy;            // Política de escritura: wt = write through, wb = write back
    long replacement_policy;      // Política de reemplazo: lru, rand
    int separated;                // Si la memory cache es separada. Nota 3.
    double access_time;
    const char* column_bit_mask;  //Esto indica que columnas se muestran y cuales se ocultan. Mascara en binario

    int num_lines;
    int num_sets;
    int num_words;
    int hex_digs_set;
    int hex_digs_line;
    int hex_digs_tag;
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
