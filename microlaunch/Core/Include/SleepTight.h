#ifndef SLEEP_TIGHT_H
#define SLEEP_TIGHT_H

//Defines the signal number used to ask microlauncher to sleep at the next metarepet
#define SIGNAL_SLEEP_TIGHT 21

//Defines the signal number used to ask microlauncher to wake up from SLEEP_TIGHT
#define SIGNAL_SLEEP_BUGBITE 22

#define PATH_TO_KILL "ResumeData/dieAlready"

/**
 * @brief Sleeps if requested so by a user
 * @return Returns 0 if the program didn't sleep, 1 otherwise
 */
int sleep_tight ();

/**
 * @brief Prepares the signal handlers needed to catch sleep/wake signals
 * @return Returns 0 in case of success, 1 otherwise
 */
int sleep_tight_preparesignals ();

/**
 * @brief Kills Microlauncher if requested so by a user (if PATH_TO_KILL exists as a file)
 */
void escape_from_scary_killers ();

#endif
