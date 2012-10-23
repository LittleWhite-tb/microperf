#ifndef H_SIGNAL
#define H_SIGNAL

#include <signal.h>

/**
 * @brief Replaces current signal handler by a new one
 * @param fct the new signal handler callback
 */
void pushSignalHandler (void (*fct) (int, siginfo_t *, void*));

/**
 * @brief Replaces current signal handler by the one that was there before (if any)
 */
void popSignalHandler (void);

#endif

