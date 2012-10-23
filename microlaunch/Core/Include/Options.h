#ifndef H_OPTIONS
#define H_OPTIONS

//Forward declaration
struct sDescription;

/**
 * @brief Check if we want the help for the options, if we do, it prints them out and returns 0, returns 1 otherwise
 * @param argc Argument count for command line
 * @param argv Argument vector for command line 
 * @return Returns 0 if no request for the help, 1 otherwise
*/
int Option_handleHelpRequest (int argc, char **argv);

/**
 * @brief Handles the command line arguments
 *
 * @param desc the Description we are about to change to handle the program arguments
 * @param argc the number of arguments passed to the program
 * @param argv the strings table of arguments passed to the program
 */
void Option_handleCommandLine (struct sDescription *desc, int argc, char **argv);

/**
 * @brief Prints the Options list
 *
 * @param name The name of the program (typically argv[0])
 */
void Option_printOptions (char *name);

#endif
