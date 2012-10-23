#ifndef H_PIPES
#define H_PIPES

#include <stdio.h>

/**
 * @struct SPipe
 * @brief overlay of the 2 tables of 2 file descriptors for communication son->father and father->son purpose
 */
typedef struct sPipe
{
	int sf[2];	/**< @brief the son->father file descriptor tube */
	int fs[2];	/**< @brief the father->son file descriptor tube */
}SPipe;

/**
 * @brief pipe () system call for the 2 tubes
 * @param pipe : the SPipe we wish to use
 */
void Pipe_generate ( SPipe *pipe );

/**
 * @brief Close the child's unused side of the pipe
 * @param pipe : the SPipe we wish to use
 * @return the two close status ; see close manpage
 */
int Pipe_closeChildUnusedSide ( SPipe *pipe );

/**
 * @brief Close the child's used side of the pipe
 * @param pipe : the SPipe we wish to use
 * @return the two close status ; see close manpage
 */
int Pipe_closeChildUsedSide ( SPipe *pipe );

/**
 * @brief Close the father's unused side of the pipe
 * @param pipe : the SPipe we wish to use
 * @return the two close status ; see close manpage
 */
int Pipe_closeFatherUnusedSide ( SPipe *pipe );

/**
 * @brief Close the father's used side of the pipe
 * @param pipe : the SPipe we wish to use
 * @return the two close status ; see close manpage
 */
int Pipe_closeFatherUsedSide ( SPipe *pipe );

/**
 * @brief Child reads inside the pipe
 * @param pipe : the SPipe we wish to use
 * @param res : the result we want to get
 * @return the size in octets that have been read from the pipe (see read manpage)
 */
ssize_t Pipe_childRead ( int pipeSF[2], int *res );

/**
 * @brief Child write inside the pipe
 * @param pipe : the SPipe we wish to use
 * @param value : the value we want to write
 * @return the size in octets that have been read from the pipe (see read manpage)
 */
ssize_t Pipe_childWrite ( int pipeFS[2], int value );

/**
 * @brief Father reads inside the pipe
 * @param pipe : the SPipe we wish to use
 * @param res : the result we want to get
 * @return the size in octets that have been read from the pipe (see read manpage)
 */
ssize_t Pipe_fatherRead ( int pipeSF[2], int *res );

/**
 * @brief Father write inside the pipe
 * @param pipe : the SPipe we wish to use
 * @param value : the value we want to write
 * @return the size in octets that have been read from the pipe (see read manpage)
 */
ssize_t Pipe_fatherWrite ( int pipeFS[2], int value );

#endif
