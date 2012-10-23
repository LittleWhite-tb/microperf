#ifndef H_BENCH_DESCRIPTOR
#define H_BENCH_DESCRIPTOR

#include "Defines.h"

unsigned long kernel1 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel2 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel3 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel4 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel5 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel6 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel7 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernel8 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);
unsigned long kernelMDL (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func);

void Dynamic_Variable_Adjustment(unsigned long size, int *step , int *nbrep);

double convert_evt(int evtnb, unsigned long evtdata, unsigned long n, unsigned long rep, unsigned long k);

#endif
