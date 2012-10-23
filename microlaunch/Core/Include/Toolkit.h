#ifndef H_TOOLKIT
#define H_TOOLKIT

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/timeb.h>
#include <sys/time.h>

#include "Defines.h"
#include "Pipes.h"

//Forward declarations
struct sDescription;

enum eToolkitError {ERROR_KEYWORD_NOT_FOUND = 0, ERROR_FILENAME_EMPTY, ERROR_FILE_NOT_FOUND, ERROR_NO_ERROR};
enum eToolkitType {INT = 0, STRING, DOUBLE};
enum eToolkitAlignment {ALIGNED = 0, NOT_ALIGNED};

/**
 * @brief Read the configuration file
 * @param szFilename filename of the configuration value
 * @param szKeyword keyword that will give us the result
 * @param pValue pointer where we fill in the information
 * @param nType type of element we are looking for
 * @return 1 on success, 0 otherwise
 */
int ReadCFG ( char * szFilename, char * szKeyword, void * pValue, int nType );

/**
 * @brief Print the configuration file
 * @param szFilename filename of the configuration value
 * @param szKeyword keyword that will give us the result
 * @param pValue pointer where we fill in the information
 * @param nType type of element we are looking for
 * @return 1 on success, 0 otherwise
 */
int ReadCFG_Print ( char * szFilename, char * szKeyword, void * pValue, int nType );

/**
 * @brief Dynamic allocation of an array with or without alignment. Allows the alignment of an array at the start of a memory page.
 *
 * @param ptr_out Pointer on the allocation (used for deallocation)
 * @param ptr_out_aligned Pointer on the aligned array
 * @param n Size required 
 * @param nAlignMode Mode of alignement (ALIGNED, NOT_ALIGNED)
 * @param nbPagesForCollisions How many pages necessary for a collision
 * @return one of the eToolkitError elements
 */
int Memory_Allocate (void ** ptr_out, void **ptr_out_aligned, size_t n, int nAlignMode, int nbPagesForCollisions);

/**
 * @brief Parses the vector surveyor line and fills the description
 * @param desc the struct sDescription that describes the program
 * @param s the vector definition
 */
void parse_vect_surveyor (struct sDescription *desc, char *s);

/**
 * @brief Parses the --nbsizes argument and sets values in the description
 * @param desc the struct sDescription that describes the program
 * @param src the source string to be parsed
 */
void parseVectorSizes (struct sDescription *desc, char *src);

/**
 * @brief Parses the --cpupin argument and sets values in the description
 * @param desc the struct sDescription that describes the program
 * @param src the source string to be parsed
 */
void parseCPUPinning (struct sDescription *desc, char *src);

/**
 * @brief Returns the cache size of the machine
 * @return Returns the cache size of the machine
 */
int getCacheSize (void);

/**
 * @brief Returns the size of the needed in regards to the specified options
 * @param desc the options description we wish to use
 * @return the size needed  for the memory allocation*/
int getMemorySpaceToAllocated (struct sDescription *desc);

/**
 * @brief Allows the program to make a barrier from the son way (child processes)
 @param pipeSF : the pipe for Son->Child communication
 @param pipeFS : the pipe for Child->Son communication */
void barrierS (int pipeSF[2], int pipeFS[2]);

/**
 * @brief Allows the program to make a barrier from the father way (master process)
 @param pipes table of pipes to communicate with every child process created
 @param nbprocess the table size to run through all the child processes created */
void barrierF (SPipe *pipes, unsigned nbprocess);

/**
 * @brief Get the number of processors available from /proc/cpuinfo
 @return the number of processors available detected from /proc/cpuinfo */
int getNbProcessorsAvailable ( void );

/**
 * @brief Check in the environment variable whether or not the current user is logged as root
 @return 1 if the user is root, else 0 */
int isUserRoot();

/**
 * @brief Microlauncher Handler for any signal we wish to capture
 * @param signal which signal made us arrive here
 * @param info the information about the signal
 * @param data additional information
 */
void SignalHandler_microlauncher (int signal, siginfo_t *info, void *data);

/**
 * @brief Microlauncher childs Handler for any signal we wish to capture (in Benchmark_Exec or Benchmark_Wrapper)
 * @param signal which signal made us arrive here
 * @param info the information about the signal
 * @param data additional information
 */
void SignalHandler_child (int signal, siginfo_t *info, void *data);

/**
 * @brief Microlauncher launching benchmark Handler for any signal we wish to capture (in benchmark_exec or benchmark_kernel)
 * @param signal which signal made us arrive here
 * @param info the information about the signal
 * @param data additional information
 */
void SignalHandler_launchingBenchmark (int signal, siginfo_t *info, void *data);

/**
 * @brief Input benchmark Handler for any signal we wish to capture (in the innermost loop of the Microlauncher, calling the kernel_run function)
 * @param signal which signal made us arrive here
 * @param info the information about the signal
 * @param data additional information
 */
void SignalHandler_benchmark (int signal, siginfo_t *info, void *data);

/**
 * @brief Child Iteration number computation
 * @param desc : The program parameters used to compute the iterations number
 * @param done : The number of iterations already done (in case of resuming)
 * @return the number of iterations the child process is going to do
 */
unsigned benchmarkIterationsNumber( struct sDescription *desc, int done);

/**
 * @brief Parses the base string and returns a table of strings containing the arguments exploded
 * @param desc : the program description used to get several information
 * @param base : the base string to be exploded, containing the whole executable arguments
 * @param argc : address of an unsigned variable which will contain the number of arguments into the program
 * @return the table of strings containing the executable arguments
 */
char **getArgs ( struct sDescription *desc, char *base, unsigned *argc );

/**
 * @brief Frees the executable arguments
 * @param argc : number of arguments to be freed
 * @param argv : arguments data to be freed
 */
void freeArgs ( unsigned argc, char **argv );

/**
 * @brief Compile the input kernel filename if any
 * @param desc : the program description used to get information
 */
void compileInputFile ( struct sDescription *desc );

/**
 * @brief strndup overlay for security purpose
 * @param src : the string to be copied
 * @param size : the maximum size of the src length
 */
char *strDuplicate (const char *src, size_t size);

/**
 * @brief Generates the XLS summary file for execution details
 * @param description the Description of the program launched
 */
int generateSummary (struct sDescription *desc);

/**
 * @brief Computes the min of a given table for unsigned long values
 * @param tab the pointer to the unsigned long table
 * @param size the number of elements of the table
 * @return the minimum value of the given table
 */
unsigned long minUL (unsigned long *tab, size_t size);

/**
 * @brief Tells whether or not the input string path corresponds to a directory
 * @param source The pointer to the string refering the path to the source to be checked
 * @return whether or not the input string path corresponds to a directory
 */
int isDirectory (const char *source);

/**
 * @brief Tells whether or not the input string path corresponds to a file
 * @param source The pointer to the string refering the path to the source to be checked
 * @return whether or not the input string path corresponds to a file
 */
int isFile (const char *source);

/**
 * @brief Parses the given directory and store output data into the given description
 * @param source The pointer to the string refering the path of the directory
 * @param desc The SDescription to be used to store data
 */
void parseDir (const char *source, struct sDescription *desc);

#endif
