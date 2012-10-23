#ifndef H_PROGRESS
#define H_PROGRESS

/**
 * @brief Prints the current progress of the execution of the benchmark
 * @param isPrintingProcess Whether or not the process has to print progress
 * @param current_step the current loop iteration for percentage computation
 * @param max the number of iterations to be done for percentage computation
 * @param overhead Whether or not the current computation is overhead computation
 * @param newline Whether or not we need a newline at the end
 */
void Benchmark_printProgress (int isPrintingProcess, unsigned current_step, unsigned max, int overhead, int newline);

/**
 * @brief Prints the current progress of the data saving process
 * @param isPrintingProcess Whether or not the process has to print progress
 * @param current_step the current loop iteration for percentage computation
 * @param max the number of iterations to be done for percentage computation
 * @param times the cross factor to be multiplied by current_step for percentage computation
 * @param times offset Defines the percent offset we want to add for percentage computation
 * @param newline Whether or not we need a newline at the end
 */
void Benchmark_printDataSavingProgress (int isPrintingProcess, unsigned current_step, unsigned max, unsigned times, unsigned offset, int newline);

/**
 * @brief Prints the launching technique (typically, Kernel or Exec mode)
 * @param isPrintingProcess Whether or not the process has to print progress
 * @param technique the technique type to be printed
 */
void Benchmark_printLaunchingTechnique (int isPrintingProcess, int technique);

/* The different modes to be handled for printing launching technique */
enum { KERNEL_MODE, EXEC_MODE };

#endif
