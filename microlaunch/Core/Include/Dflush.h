#ifndef H_DFLUSH
#define H_DFLUSH

#include <stdio.h>

/**
 * @brief Clear the caches by reading an inconsistent array
 * @param dummy : the inconsistent array we want to read
 * @param size_MB : the number of elements of the dummy array we want to read (in bytes)
 */
double readDummyArray (double *dummy, unsigned long sizeMB);

#endif
