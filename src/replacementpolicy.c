#include "computer.h"
#include "datainterface.h"
#include "replacementpolicy.h"


/* Private functions */
int replacement_lru(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line);
int replacement_lfu(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line);
int replacement_random(int first_line, int last_line);
int replacement_fifo(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line);


/**
 * @brief Picks a cache line to be replaced according to the replacement policy. By default, if a line is not valid, it gets replaced regardless of the policy
 * @param computer The computer
 * @param instructionOrData If the operation stores instructions or data
 * @param cacheLevel The level of the cache to operate in
 * @param set The line of the cache
 */
int select_line_to_replace(Computer *computer, int instructionOrData, int cacheLevel, int set) {
    Cache *cache = &computer->cache[cacheLevel];

    //The first and last lines of the set are calculated
    int first_line = set * cache->associativity;
    int last_line = first_line + cache->associativity;

    switch (cache->replacement_policy) {
        case LRU:
            return replacement_lru(computer, instructionOrData, cacheLevel, first_line, last_line);

        case LFU:
            return replacement_lfu(computer, instructionOrData, cacheLevel, first_line, last_line);

        case RANDOM:
            return replacement_random(first_line, last_line);

        case FIFO:
            return replacement_fifo(computer, instructionOrData, cacheLevel, first_line, last_line);

        default:
            return set * cache->associativity;          // If the replacement policy is not defined, replace the first line in the set
    }
}


/**
 * @brief Applies the Least Recently Used replacement policy.
 * @param cache The cache that contains the data. The associativity is checked to determine the size of the set
 * @param set The set to operate on
 * @return The line that has been picked to be replaced
 */
int replacement_lru(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line) {
    Cache *cache = &computer->cache[cacheLevel];
    CacheLineContent cacheData;

    int lruLine = -1;
    int lruTime = -1;

    // The set is iterated to find the line that should be replaced
    for (int line = first_line; line <= last_line; line++){
        // The line is read
        read_line_from_cache(computer, instructionOrData, cacheLevel, &cacheData, line);

        // If the line contains invalid data, it gets replaced by default
        if (cacheData.valid == 0){
            return line;
        }

        // If there is not a candidate for replacement or the line has been accessed less than the previous ones, it gets updated
        if(lruLine == -1 || lruTime > cacheData.lastAccess) {
            lruLine = line;
            lruTime = cacheData.lastAccess;
        }
    }
    return lruLine;
}


/**
 * @brief Applies the Least Frequently Used replacement policy.
 * @param cache The cache that contains the data. The associativity is checked to determine the size of the set
 * @param set The set to operate on
 * @return The line that has been picked to be replaced
 */
int replacement_lfu(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line) {
    Cache *cache = &computer->cache[cacheLevel];
    CacheLineContent cacheData;

    int lfuLine = -1;
    int lfuCount = -1;

    // The set is iterated to find the line that should be replaced
    for (int line = first_line; line <= last_line; line++){
			// The line is read
        read_line_from_cache(computer, instructionOrData, cacheLevel, &cacheData, line);

			// If the line contains invalid data, it gets replaced by default
        if (cacheData.valid == 0){
            return line;
			}

			// If there is not a candidate for replacement or the line has been accessed less than the previous ones, it gets updated
        if (lfuLine == -1 || lfuCount < cacheData.accessCount) {
            lfuLine = line;
            lfuCount = cacheData.accessCount;
        }
    }
    return lfuLine;
}


/**
 * @brief Applies the random replacement policy.
 * @param cache The cache that contains the data. The associativity is checked to determine the size of the set
 * @param set The set to operate on
 * @return The line that has been picked to be replaced
 */
int replacement_random(int first_line, int last_line) {
        return first_line + rand() % (first_line - last_line);
}



/**
 * @brief Applies the First In First Out replacement policy.
 * @param cache The cache that contains the data. The associativity is checked to determine the size of the set
 * @param set The set to operate on
 * @return The line that has been picked to be replaced
 */
int replacement_fifo(Computer *computer, int instructionOrData, int cacheLevel, int first_line, int last_line) {
    Cache *cache = &computer->cache[cacheLevel];
    CacheLineContent cacheData;

    int fifoLine = -1;
    int fifoTime = -1;

    // The set is iterated to find the line that should be replaced
    for (int line = first_line; line <= last_line; line++){
			// The line is read
        read_line_from_cache(computer, instructionOrData, cacheLevel, &cacheData, line);

			// If the line contains invalid data, it gets replaced by default
        if (cacheData.valid == 0){
            return line;
			}

			// If there is not a candidate for replacement or the selected line is newer than the current one, the line gets updated
        if (fifoLine == -1 || fifoTime < cacheData.firstAccess) {
            fifoLine = line;
            fifoTime = cacheData.firstAccess;
        }
    }
    return fifoLine;
}
