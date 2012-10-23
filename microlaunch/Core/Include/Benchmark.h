#ifndef H_BENCHMARK
#define H_BENCHMARK

#include "Defines.h"
#include <stdio.h>
#include "Pipes.h"

//Advance declaration
struct sDescription;

/**
 * @brief the kernel function pointeur type, need for the associative table nbflux => kernel to use
 */
typedef unsigned long (*kernel_fctptr) (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func); 

/*============= GLOBAL CONSTANTS ===========================================*/
enum CostCalculation {ITERATION_COST = 0, FUNCTION_COST, RAW_NUMBERS};

enum ResultError { NO_ERROR = 0, OVERHEAD_TOO_HIGH };

/**
 * @brief Computes the number of experiments we want to make
 * @param desc the Description structure we wish to use
 * @return Returns the number of experiments we want to make
 */
int getExperimentNumber ( struct sDescription *desc );


/**
 * @brief Launch the 2D benchmark
 * @param desc the struct sDescription we wish to use
 * @return returns EXIT_SUCCESS/EXIT_FAILURE depending on the status of the execution
 */
int Benchmark_Wrapper (struct sDescription *desc, int currentExecRepet);

/**
 * @brief Does the filename contain .cfg or not
 * @param s the string representing the filename
 * @return whether or not the filename contains .cfg
 */
int containsCFG(char *s);

/**
 * @brief compute the next systemState of benchmark
 * @param desc the description we wish to use
 * @param systemState containt the evolution of all run possibilities
 * */
void Benchmark_next(struct sDescription *desc,int *systemState);

/**
 * @brief initializes the CSV
 * @param desc the description we wish to use
 * @param stream the file descriptor inside which we want to write
 * */
void Benchmark_initCsv (struct sDescription *desc, FILE *stream);

/**
 * @brief print in a csv file all results we wish to expoit
 * @param res the results we wish to print
 * @param nbEvalLibs the number of evaluation librairies to be written
 * @param currentMetaRepet the ID of the current meta-repetition to be written
 * @param offsets the array of alignments to be stored
 * @param nb_offsets the number of vectors used in the program
 * @param nb_resumes the number of times the program was "--resume"d
 * @param currun the id of the current run
 * @param stream the file descriptor pointer we wish to use
 * @param problem if problem != 0, the current computation process have had some trouble
 * */
void Benchmark_printCsv(BenchResult **res, unsigned nbEvalLibs, unsigned currentMetaRepet, int* offsets,
			unsigned nb_offsets, int nb_resumes, int currun, FILE *stream, int problem);

/**
 * @brief Allocate the dummy array
 * @param desc the SDescription describing the program
 */
void Benchmark_makeDummyArray (struct sDescription *desc);

/**
 * @brief Creates and open the output CSV file we want to write into for both kernel and exec mode
 * @param desc the description of the program
 * @param currentExecRepet the current execute-repetition of the program
 * @param currentVectorSize the current vector size of the arrays (ignored in Exec mode)
 * @return the file descriptor we are going to use
 */
FILE *Benchmark_createOutputFile (struct sDescription *desc, int currentExecRepet, int currentVectorSize);

#endif

