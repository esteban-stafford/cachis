#include "writepolicy.h"
#include "datamanipulation.h"

void write_back(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int cacheLine) {
    printf("\n-> Applying WriteBack in cache\n");

    // The contents of the line get read
    CacheLineContent content;
    read_line_from_cache(computer, operation->instructionOrData, 0, &content, cacheLine);

    int position = operation->address % (computer->cache[0].num_words * 4) / 4;
    for (unsigned i = 0; i < response->size; i++) {
		content.content[position + i] = operation->data;
	}

	content.dirty = 1;

    // The line is written back to the cache
    write_line_to_cache(computer, operation->instructionOrData, 0, &content, cacheLine);
}

void write_through(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response) {
    printf("\n-> Applying WriteThrough in memory\n");
    MemoryPosition pos;
	FILE *file;
	int lastNumber;

	// If the DRAMSys trace has been requested, the related variables get initiated
	if (generate_dramsys_trace) {
		lastNumber = open_dramsys_file(dramsys_file, &file);
	}

    // The address and content gets noted
    pos.address = operation->address;
    pos.content = operation->data;

    // The access to memory gets noted
    stats->numAccesses[MAX_CACHES] += response->size;

    // Write data from request into memory
    // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
    // After every iteration, the address increases one word
    for (unsigned i = 0, address=response->address; i < response->size; i++, address+=computer->cpu.word_width/8) {
        pos.content = response->data[i];
		pos.address = address;
        if (write_to_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
        }

		// If the DRAMSys trace has been requested, append a write for the current address
		if (generate_dramsys_trace) {
			write_to_dramsys_file(&file, lastNumber + i, 1, address);
		}

        // If it's not the first access, it gets noted as a burst access
        if (i != 0) {
			stats->numBurstAccesses++;
		}
    }

    // The file gets closed if it has been opened previously
	if (generate_dramsys_trace) {
		close_dramsys_file(&file);
	}

}
