/*============================================================================
   PROJECT : Kernel_Mem_Benchmark (kerbe)
   SOURCE  : Defines.h
   MODULE  : Contient tous les defines de l'exp�ience
   AUTHOR  : Christophe Lemuet
   UPDATED : 02/05/2003 (DD/MM/YYYY)
============================================================================*/

#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifdef X86
	#define PROC_STRING "X86"
#else
    #error "Only supporting X86"
#endif

#define ALIGNEMENT ALIGNED      

/**
 * @todo Move this to a separate .h
 */
#include <stdint.h>


/**
 * @brief struct sBenchResult contains the results of a benchmark program run
 */
typedef struct sBenchResult 
{
	double *time;            /**< @brief Time spent by the benchmark program */
    uint64_t *iterations;    /**< @brief Number of iterations */
    double initialTime;		/**< @brief Result of the start call */
    double finalTime;		/**< @brief Result of the stop call */
} BenchResult;

#define STRBUF_MAXLEN 512

#endif


