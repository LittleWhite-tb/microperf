#include <stdio.h>
unsigned long entryPoint ( unsigned long n, void *x, size_t elem_size )
{
	unsigned long i;
	//Registers declaration
	register float x0 asm ("%xmm0");
	register float *r0 asm ("%rdi");
	r0 = x;
	#pragma omp for 
	for (i = 0; i < 20; i += (1 * 4))
	{

		__asm__ __volatile__ ("movss %1, %0" : "=x" (x0) : "m" (*(r0 + (i + 0))));
		__asm__ __volatile__ ("movss %1, %0" : "=x" (x0) : "x" (x0));
	tmpsum[omp_get_thread_num ()] += x0;
		__asm__ __volatile__ ("#Unrolled factor 1");

	}
	return ((20 - 0) / (1 * 4));
}
