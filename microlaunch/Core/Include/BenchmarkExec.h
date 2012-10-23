#ifndef H_BENCHMARK_EXEC
#define H_BENCHMARK_EXEC

#include <stdio.h>
#include "Defines.h"
#include "Description.h"

/**
 * @brief Benchmark launching executable
 * @param desc : The program parameters to be used
 * @return EXIT_SUCCESS/EXIT_FAILURE depending on the status of the execution
 */
int Benchmark_Exec ( struct sDescription *desc, int currentExecRepet );

/**
 * @brief Initializes the output CSV file (if any)
 * @param desc The SDescription describing the program
 * @param stream The file descriptor pointing to the output CSV file
 */
void BenchmarkExec_initCsv (SDescription *desc, FILE *stream);

/**
 * @brief Prints data into the output CSV file
 * @param res The table of results describing the execution
 * @param nbEvalLibs The number of evaluation librairies to be written
 * @param currentMetaRepet the id of the current meta-repetition to be written
 * @param stream the file descriptor pointing to the output CSV file
 * @param problem Whether or not a problem occured during this meta-repetition
 */
void BenchmarkExec_printCsv (BenchResult **res, unsigned nbEvalLibs, unsigned currentMetaRepet, FILE *stream, int problem);

#endif
