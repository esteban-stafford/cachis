#include <stdio.h>
#include <string.h>
#include <math.h>
#include "confparser.h"
#include "datainterface.h"
#include "datamanipulation.h"
#include "simulator.h"
#include "mappingpolicy.h"
#include "replacementpolicy.h"
#include "statistics.h"

unsigned long cycle = 0;

/* Private functions */
void find_in_cache(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int topLevel);
void read_from_memory(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response);
void populate_cache(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int topLevel);
void move_to_lower_level(Computer *computer, Stats *stats, int instructionOrData, int cacheLevel, int line);
void get_mapping(Computer *computer, int cacheLevel, MemoryOperation *operation, MappingResult *mappingResult);


/**
 * @brief Executes all the memory operations in one go
 */
void simulate(Computer *computer) {
    for(int i=0; i<numberOfOperations; i++){
        simulate_step(computer, &memoryOperations[i]);
        fflush(stdout);
    }
}


/**
 * @brief Simulates a single step
 * @param computer The computer to execute the operation
 * @param operation The operation to be executed
 */
void simulate_step(Computer *computer, MemoryOperation *operation) {
    printf("\n----> Cycle %lu\n\n", cycle);

    // The response gets initiated
    ResponseType response;
    response.size = operation->size/4;
    response.address = operation->address;
    response.data = malloc((sizeof(unsigned)));
    response.data[0] = operation->data;
    response.resolved = -1;

    Stats stats;
    init_statistics(&stats);


    printf("Simulating operation: ");
    printMemOperation(stdout, operation, computer->cpu.address_width);
    increment_integer_statistics("CPU", "Accesses", 1);

    // If the operation is a LOAD
    if (operation->operation == LOAD) {
        // Try to find the value in the caches
        find_in_cache(computer, operation, &stats, &response, 0);

        // If no cache level resolved the request, the memory gets accessed
        if (response.resolved < 0) {
            read_from_memory(computer, operation, &stats, &response);

        }

        // Lastly, since the data has been requested by the CPU, all levels above response.resolved have to been populated
        // until it reaches the top level the CPU has access to.
        populate_cache(computer, operation, &stats, &response, 0);

    } else {            // If the operation is a STORE
        // If the policy of the first level is WRITE_BACK
        if (computer->cache[0].write_policy == WRITE_BACK) {
            // The whole cache gets checked to see if the data is available
            find_in_cache(computer, operation, &stats, &response, 0);

            // If no cache level resolved the request, the memory gets accessed
            if (response.resolved < 0) {
                read_from_memory(computer, operation, &stats, &response);
            }

            // The top levels get populated
            populate_cache(computer, operation, &stats, &response, 0);

            // The last level gets updated with the operation's data and the number of accesses gets incremented by 1
            write_back(computer, operation, &stats, &response, response.cacheLineDest[0]);
            stats.numAccesses[0]++;

        } else if (computer->cache[0].write_policy == WRITE_THROUGH) {
            // The content gets directly written to memory and the memory gets accessed once
            write_through(computer, operation, &stats, &response);

            // Read the data and populate all caches with it
            read_from_memory(computer, operation, &stats, &response);
            populate_cache(computer, operation, &stats, &response, 0);
        }
    }

    // The action gets printed

    if (operation->operation == LOAD) {
        printf("\nFinished simulation, the response contains: 0x%x\n",response.data[0]);        // The data that was operated with
    } else {
        printf("\nFinished simulation.\n");
    }

    free(response.data);

    // The statistics get calculated and printed
    update_statistics(computer, &stats);
    print_statistics(stdout);

    printf("\n\n<<----------------------------------------------->>\n\n");
    cycle++;
}

/**
 * @brief Iterates through all caches to find if a value is in cache and updates response. If the data is not in cache, the response's capacity gets modified to
 *        house a full cache line.
 * @param computer The computer.
 * @param operation The operation to perform on the caches.
 * @param charName Printinf information.
 * @param response Statistics about the access. Will get updated with time and the level of cache that contains the data.
 * @param topLevel The level from where the search starts
 */
void find_in_cache(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int topLevel) {
    printf("\n-> Looking in cache\n");

        // Iterate through all the cache levels
    for(int cacheLevel = topLevel; cacheLevel < computer->num_caches; cacheLevel++){
        MappingResult mappingResult;
        long line;

        // The mapping gets calculated
        get_mapping(computer, cacheLevel, operation, &mappingResult);

        // The number of accesses gets incremented by one
        stats->numAccesses[cacheLevel]++;

        // Find tag in cache
        line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, mappingResult.set, mappingResult.tag);

        // If there was an error, print it and return without making changes
        if (line == -2){
            printf("\t Error: The level of the cache was lower than 0 or exceeded the maximum.\n");
            return;
        }

        // If there is a hit and the address has been found
        if (line > -1) {
            // Hit
            printf("\t > Hit in L%d cache. Line %ld has the data.\n", cacheLevel + 1, line);

            // The statistics get updated
            stats->numHits[cacheLevel]++;

            // A response gets prepared
            CacheLineContent cacheData;
            cacheData.content = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);

            // Read data from cache into response
            read_line_from_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);
            if (response->size == 1) {
                response->data[0] = cacheData.content[mappingResult.offset];
                printf("\t The first element contained in offset %d is 0x%x\n",mappingResult.offset, response->data[0]);
            }

            // Remember cache level and line that resolved the request
            response->resolved = cacheLevel;
            response->cacheLineDest[cacheLevel] = line;

            // Since there has been a hit, there's no need to reach the lower levels of the cache, the loop ends
            return;
        } else {     //If there is a miss
            // Miss
            // printf(">    %s: Miss 2^%d-1 = %f\n", cacheName,computer->cache[cacheLevel].offset_bits, pow(2,computer->cache[cacheLevel].offset_bits)-1);
            printf("\t > Miss in L%d cache.\n", cacheLevel + 1);

            // The statistics get updated
            stats->numMisses[cacheLevel]++;

            // Upgrade request to a full cache line
            response->size = computer->cache[cacheLevel].num_words;
            response->address &= -1 << computer->cache[cacheLevel].offset_bits;
            free(response->data);
            response->data = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);
        }
    }
    printf("\t Data has not been found in cache.\n");
}


/**
 * @brief Accesses memory to load or store data.
 * @param computer The computer.
 * @param operation The operation to perform on the memory.
 * @param charName Printinf information.
 * @param response Statistics about the access.
 */
void read_from_memory(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response) {
    printf("\n-> Reading memory\n");

    MemoryPosition pos;

    // Remember that the memory resolved the request
    // The number of caches is used to signify that it has been through all of them
    response->resolved = computer->num_caches;

    // Statistics get updated
    stats->numAccesses[MAX_CACHES] += response->size;

    // And the data is read from memory
    // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
    // After every iteration, the address increases one word
    for (unsigned i=0, address=response->address; i < response->size; i++, address+=computer->cpu.word_width/8) {
        if (read_from_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
        }
        response->data[i] = pos.content;
    }
}

/**
 * @brief Populates all caches above the line that resolved the request
 * @param computer The computer.
 * @param operation The operation to perform on the caches.
 * @param charName Printinf information.
 * @param response Statistics about the access.
 * @param topLevel The last level that will get populated.
 */
void populate_cache(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response, int topLevel) {
    // If the level that resolved the response is below the top one, the caches get populated
    if (response->resolved > 0) {
        printf("\n-> Populating caches\n");
    }

    // Iterate backwards through all the cache levels that were involved in request
    for (int cacheLevel = response->resolved-1; cacheLevel >= topLevel; cacheLevel--){
        MappingResult mappingResult;
        long line;

        // Calculate the different fields from the address
        get_mapping(computer, cacheLevel, operation, &mappingResult);

        // Find tag in cache
        line = find_tag_in_cache(computer, operation->instructionOrData, cacheLevel, mappingResult.tag, mappingResult.set);

        // If there was an error, print it and return without making changes
        if(line == -2){
            printf("Error: The level of the cache was lower than 0 or exceeded the maximum.\n");
            return;
        }

        // If the line was found in cache, there's a hit
        // TODO consider that the line can be dirty and should be witten to the next level
        if(line > 0) {
            // Hit
            printf("\t < Hit in L%d cache. Line %ld has the data.\n", cacheLevel + 1, line);
        } else {         //If not, miss
            printf("\t < Miss in L%d cache. Populating.\n", cacheLevel + 1);
            // Load operation
            CacheLineContent cacheData, existingData;
            cacheData.dirty = 0;
            cacheData.valid = 1;
            cacheData.tag = mappingResult.tag;
            cacheData.content = response->data;
			cacheData.startingAddress = response->address;

            // The via gets located and populated with the data
            int line = select_line_to_replace(computer, operation->instructionOrData, cacheLevel, mappingResult.set);

			// If the line contains data and is dirty, it gets written to the level below
			read_flags_from_cache(computer, operation->instructionOrData, cacheLevel, &existingData, line);

			if (existingData.dirty == 1) {
				move_to_lower_level(computer, stats, operation->instructionOrData, cacheLevel, line);
			}
			write_line_to_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);

			// The line that contains the data on the current level gets noted
			response->cacheLineDest[cacheLevel] = line;
		}
    }
}


/**
 * @brief Populates the level that is below the cacheLevel specified
 * @param computer The computer.
 * @param instructionOrData If the cache contains instructions or data
 * @param cacheLevel The cache level that contains the data
 * @param line The line that has to be moved
 */
void move_to_lower_level(Computer *computer, Stats *stats, int instructionOrData, int cacheLevel, int line) {
	CacheLineContent existingData;
	read_line_from_cache(computer, instructionOrData, cacheLevel, &existingData, line);

	// All the words in the line have to get moved to a lower level
	// A new MemoryOperation gets created
	MemoryOperation moveOp;
	moveOp.address = existingData.startingAddress;				// The address that contains the modified data in memory is noted
	moveOp.instructionOrData = instructionOrData;
	moveOp.size = computer->cache[cacheLevel].num_words; 		// All the lines in the current cache have to be moved

	// A new response gets created
	ResponseType moveResponse;
	moveResponse.size = moveOp.size;
	moveResponse.address = moveOp.address;
	moveResponse.resolved = cacheLevel;

	// The entire line gets saved in the data variable
	moveResponse.data = malloc((sizeof(unsigned) * moveResponse.size));
	for (int i = 0; i < moveResponse.size; i++) {
		moveResponse.data[i] = existingData.content[i];
	}

	// When the actual stats shouldn't be updated, this empty stats struct should be used
	Stats fillerStats;

	printf("\t Collision detected! ");

	// Move to memory if the last level has been reached
	if (computer->num_caches - 1 <= cacheLevel) {
		printf("Reached memory, writing directly\n");
		write_through(computer, &moveOp, stats, &moveResponse);
	} else {
		switch (computer->cache[cacheLevel - 1].write_policy) {
			case WRITE_THROUGH:
				// The contents get written to memory and propagated up to cacheLevel - 1
				printf("Moving to memory (Lower level is WT)\n");
				write_through(computer, &moveOp, stats, &moveResponse);

				// Read the data and populate all caches with it
				read_from_memory(computer, &moveOp, &fillerStats, &moveResponse);
				populate_cache(computer, &moveOp, &fillerStats, &moveResponse, cacheLevel - 1);
				break;

			case WRITE_BACK:
				printf("Moving to lower level (Lower level is WB)\n");

				// The whole cache gets checked to see if the data is available
				find_in_cache(computer, &moveOp, &fillerStats, &moveResponse, cacheLevel - 1);

				// If no cache level resolved the request, the memory gets accessed
				if (moveResponse.resolved < 0) {
					read_from_memory(computer, &moveOp, &fillerStats, &moveResponse);
				}

				// The top levels get populated
				populate_cache(computer, &moveOp, &fillerStats, &moveResponse, cacheLevel - 1);

				// The last level gets updated with the moveOp's data and the number of accesses gets incremented by 1
				write_back(computer, &moveOp, stats, &moveResponse, moveResponse.cacheLineDest[cacheLevel - 1]);
				break;
		}
	}
}

/**
 * @brief Calculates the mapping and stores it in mappingResult
 * @param computer The void get_mapping(Computer* computer, int cacheLevel, MemoryOperation* operation, MappingResult* mappingResult)
 * @param cacheLevel The cache level that is being checked
 * @param operation The memory void get_mapping(Computer* computer, int cacheLevel, MemoryOperation* operation, MappingResult* mappingResult)
 * @param mappingResult Pointer to the struct that will get updated
 */
void get_mapping(Computer* computer, int cacheLevel, MemoryOperation* operation, MappingResult* mappingResult) {
     Cache cache = computer->cache[cacheLevel];

     if (cache.associativity == 1) {                                                    // Direct Mapping
          direct_associative(&cache, operation, mappingResult);
     } else if (cache.associativity > 1) {                                              // N way set associative
          set_associative(&cache, operation, mappingResult);
     } else {                                                                           //Fully associative
          fully_associative(&cache, operation, mappingResult);
     }
}
