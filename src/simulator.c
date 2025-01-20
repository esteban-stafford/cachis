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
void populate_cache(Computer *computer, MemoryOperation *operation, ResponseType *response, int topLevel);
void move_to_lower_level(Computer *computer, int instructionOrData, int cacheLevel, int line);
void upgrade_response_to_full_line(Computer *computer, int cacheLevel, ResponseType *response);
void get_mapping(Computer *computer, int cacheLevel, MemoryOperation *operation, MappingResult *mappingResult);


/**
 * @brief Executes all the memory operations in one go
 */
void simulate(Computer *computer) {
    for(int i=0; i<numberOfOperations; i++){
        simulate_step(computer, &memoryOperations[i]);
        fflush(stdout);
    }

    // Print the memory at the end of the simulation
    print_memory_contents(computer);
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
        populate_cache(computer, operation, &response, 0);

    } else {            // If the operation is a STORE
        // If the policy of the first level is WRITE_BACK
		switch (computer->cache[0].write_policy) {
			case WRITE_BACK:
				// A backup of the original response size is saved for the write_back function
				ResponseType write_response;
				write_response.size = response.size;

				// The whole cache gets checked to see if the data is available
				find_in_cache(computer, operation, &stats, &response, 0);

				// If no cache level resolved the request, the memory gets accessed
				if (response.resolved < 0) {
					read_from_memory(computer, operation, &stats, &response);
				}

				// The top levels get populated
				populate_cache(computer, operation, &response, 0);

				// The response gets set to the original size
				response.size = write_response.size;

				// The last level gets updated with the operation's data and the number of accesses gets incremented by 1
				write_back(computer, operation, &response, response.cacheLineDest[0]);
				break;
			case WRITE_THROUGH:
				// The content gets directly written to memory and the memory gets accessed once
				write_through(computer, operation, &response);

				// The response gets upgraded to house an entire cache line of data
				upgrade_response_to_full_line(computer, computer->num_caches - 1, &response);

				// Read the data and populate all caches with it
				read_from_memory(computer, operation, &stats, &response);
				populate_cache(computer, operation, &response, 0);
				break;
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
    for (int cacheLevel = topLevel; cacheLevel < computer->num_caches; cacheLevel++) {
        MappingResult mappingResult ;
        long line;

        // The mapping gets calculated
        get_mapping(computer, cacheLevel, operation, &mappingResult);

        // The number of accesses gets incremented by one
		if (stats != NULL) {
			stats->numAccesses[cacheLevel]++;
		}


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
			if (stats != NULL) {
				stats->numHits[cacheLevel]++;
			}

            // A response gets prepared
            CacheLineContent cacheData;
            cacheData.content = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);

            // Read data from cache into response
            read_line_from_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);
            for (int i = 0; i < response->size; i++) {
				response->data[i] = cacheData.content[mappingResult.offset + i];
			}

			printf("\t The cache line contains: ");
			for (int i = 0; i < computer->cache[cacheLevel].num_words; i++) {
					printf("0x%x ", cacheData.content[i]);
			}
			printf("\n");

            // Remember cache level and line that resolved the request
            response->resolved = cacheLevel;
            response->cacheLineDest[cacheLevel] = line;

            // Since there has been a hit, there's no need to reach the lower levels of the cache, the loop ends
            return;
        } else {     //If there is a miss
            // Miss
            printf("\t > Miss in L%d cache.\n", cacheLevel + 1);

            // The statistics get updated
			if (stats != NULL) {
				stats->numMisses[cacheLevel]++;
			}


            // Upgrade request to a full cache line
			upgrade_response_to_full_line(computer, cacheLevel, response);
        }
    }
    printf("\t Data has not been found in cache.\n");
}


/**
 * @brief Accesses memory and reads data from an operation.
 * @param computer The computer.
 * @param operation The operation to perform on the memory.
 * @param stats Global statistics. The number of accesses to the memory will be updated
 * @param response Container for the data that has been read from memory.
 */
void read_from_memory(Computer *computer, MemoryOperation *operation, Stats *stats, ResponseType *response) {
    printf("\n-> Reading memory\n");

    MemoryPosition pos;
	FILE *file = NULL;
	int lastNumber;

	// If the DRAMSys trace has been requested, the related variables get initiated
	if (generate_dramsys_trace) {
		lastNumber = open_dramsys_file(dramsys_file, &file);

		// Since this is a different burst, there has to be some separation between the last timestamp
		lastNumber++;
	}

    // Remember that the memory resolved the request
    // The number of caches is used to signify that it has been through all of them
    response->resolved = computer->num_caches;

    // Statistics get updated
	if (stats != NULL)  {
		stats->numAccesses[MAX_CACHES] += response->size;
	}


    // And the data is read from memory
    // After every iteration the address gets incremented by computer->cpu.word_width / 8 (Converts the word size to bytes)
    // After every iteration, the address increases one word
    for (unsigned i = 0, address = response->address; i < response->size; i++, address += computer->cpu.word_width/8) {
        if (read_from_memory_address(computer, &pos, address) < 0) {
            fprintf(stderr, "error in simulation: %s addr:%x\n", interfaceError, address);
            return;
        }

		// If the DRAMSys trace has been requested, append a read for the current address
		if (generate_dramsys_trace) {
			write_to_dramsys_file(&file, lastNumber + i, 0, address);
		}

        // If it's not the first access, it gets noted as a burst access
        if (i != 0 && stats != NULL) {
			stats->numBurstAccesses++;
		}

        response->data[i] = pos.content;
    }

    // The file gets closed if it has been opened previously
	if (generate_dramsys_trace) {
		close_dramsys_file(&file);
	}
}

/**
 * @brief Populates all caches above the line that resolved the request
 * @param computer The computer.
 * @param operation The operation to perform on the caches.
 * @param response Contains the data that will get propagated to the upper levels.
 * @param topLevel The last level to be populated.
 */
void populate_cache(Computer *computer, MemoryOperation *operation, ResponseType *response, int topLevel) {
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
        if (line > 0) {
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
				move_to_lower_level(computer, operation->instructionOrData, cacheLevel, line);
			}
			write_line_to_cache(computer, operation->instructionOrData, cacheLevel, &cacheData, line);

			// The line that contains the data on the current level gets noted
			response->cacheLineDest[cacheLevel] = line;
		}
    }
}


/**
 * @brief Moves data to lower levels of the hierarchy. Called when there is a collision.
 * @param computer The computer.
 * @param instructionOrData If the data is located in an instruction or data cache.
 * @param cacheLevel The cache level that contains the data that has to be moved.
 * @param line The line that has to be moved.
 */
void move_to_lower_level(Computer *computer, int instructionOrData, int cacheLevel, int line) {
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

	// The line is unmarked as dirty
	existingData.dirty = 0;
	write_flags_to_cache(computer, instructionOrData, cacheLevel, &existingData, line);

	printf("\t Collision detected, moving to lower level. ");

	// Move to memory if the last level has been reached
	if (computer->num_caches - 1 <= cacheLevel) {
		printf("Reached memory, writing directly.\n\n");
		printf("-> Starting move to lower levels:\n");
		write_through(computer, &moveOp, &moveResponse);
	} else {
		switch (computer->cache[cacheLevel + 1].write_policy) {
			case WRITE_THROUGH:
				// The contents get written to memory and propagated up to cacheLevel - 1
				printf("Moving existing data to memory (Lower level is WT)\n\n");
				printf("-> Starting move to lower levels:\n");
				write_through(computer, &moveOp, &moveResponse);

				// Read the data and populate all caches with it
				read_from_memory(computer, &moveOp, NULL, &moveResponse);
				populate_cache(computer, &moveOp, &moveResponse, cacheLevel + 1);
				break;

			case WRITE_BACK:
				printf("Moving existing data to L%d (Lower level is WB)\n\n", cacheLevel + 1);
				printf("-> Starting move to lower levels:\n");

				// The whole cache gets checked to see if the data is available
				find_in_cache(computer, &moveOp, NULL, &moveResponse, cacheLevel + 1);

				// If no cache level resolved the request, the memory gets accessed
				if (moveResponse.resolved < 0) {
					read_from_memory(computer, &moveOp, NULL, &moveResponse);
				}

				// The top levels get populated
				populate_cache(computer, &moveOp, &moveResponse, cacheLevel - 1);

				// The last level gets updated with the moveOp's data and the number of accesses gets incremented by 1
				write_back(computer, &moveOp, &moveResponse, moveResponse.cacheLineDest[cacheLevel - 1]);
				break;
		}
	}
	printf("\n-> Finishing move to lower levels\n\n");
}

/**
 * @brief Allocates memory and expands a request to house all the data inside a full cache line.
 * @param computer The computer.
 * @param cacheLevel The cache level that will be checked to determine the size of the line.
 * @param response The response.
 */
void upgrade_response_to_full_line(Computer *computer, int cacheLevel, ResponseType *response){
	response->size = computer->cache[cacheLevel].num_words;
	response->address &= -1 << computer->cache[cacheLevel].offset_bits;
	free(response->data);
	response->data = malloc((sizeof(long))*computer->cache[cacheLevel].num_words);
}

/**
 * @brief Calculates the mapping and stores it in mappingResult
 * @param computer The computer
 * @param cacheLevel The cache level that is being checked
 * @param operation The memory operation.
 * @param mappingResult Contains the results of the mapping
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
