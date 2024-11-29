#include "writepolicy.h"

void write_back(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int cacheLine) {
    // The contents of the line get read
    CacheLineContent content;
    read_line_from_cache(computer, operation->instructionOrData, 0, &content, cacheLine);

    // Update the contents of the line
    content.content[cacheLine] = *response->data;
    content.dirty = 1;

    // The line is written back to the cache
    write_line_to_cache(computer, operation->instructionOrData, 0, &content, cacheLine);

    // The statistics get updated
    stats->numAccesses[0]++;


}

void write_through(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response) {
    MemoryPosition pos;

    // The address and content gets noted
    pos.address = operation->address;
    pos.content = operation->data;

    // The access to memory gets noted
    stats->numAccesses[MAX_CACHES]++;

    // Write data from request into memory
    // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
    // After every iteration, the address increases one word
    for (unsigned i = 0, address=response->address; i < response->size; i++, address+=computer->cpu.word_width/8) {
        pos.content = response->data[i];
        if (write_to_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
        }
    }
}
