/*
Copyright (C) 2011 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/io.h>
#include <unistd.h>

#include "Benchmark.h"
#include "BenchmarkExec.h"
#include "Defines.h"
#include "Description.h"
#include "Dflush.h"
#include "Log.h"
#include "Progress.h"
#include "Signal.h"
#include "Toolkit.h"

/**
 * @brief Executes the fileName program
 * @param fileName : the file name we wish to execute
 * @param desc : The description is set to get the fileName arguments
 * @param overhead : whether or not the current execution is overhead computation
 * @param isPrintingProcess : whether or not the current process has to print something
 * @return the exit status of the fileName execution
 */
int
execLauncher ( SDescription *desc, char *fileName, int overhead, int isPrintingProcess )
{
	int fd;
	pid_t pid;
	int res;
	char *redirect;
	
	pushSignalHandler (SignalHandler_benchmark);
	
	switch (pid = fork ())
	{
		case -1:
		{
			perror ("Error when forking in benchmark");
			exit (EXIT_FAILURE);
		}
		case 0:
		{
			// Output handling : Does the exec has to print something ?
			if (!isPrintingProcess || Description_getSuppressOutput (desc) == 1)
			{
				fclose (stdout);
				fclose (stderr);
			}
			
			redirect = Description_getOutputFileStream (desc);
			/* If redirect is defined, then let's redirect executable's output */
			if (isPrintingProcess && overhead == 0 && redirect != NULL)
			{
				/* Opening file descriptor */
				if((fd = open(redirect, O_APPEND|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP)) == -1) { /*open the file */
					perror("Error while opening file descriptor");
					abort ();
				}
				
				/* Duping stdout to our file descriptor */
				if (dup2 (fd, STDOUT_FILENO) == -1)
				{
					Log_output (-1, "An error occured while dup2ing stdout...\n");
					perror("");
					abort ();
				}
				
				/* Duping stderr to our file descriptor */
				if (dup2 (fd, STDERR_FILENO) == -1)
				{
					Log_output (-1, "An error occured while dup2ing stderr...\n");
					perror ("");
					abort ();
				}
				
				/* We don't need our file descriptor anymore, so we close it */
				if (close (fd) == -1)
				{
					perror ("Error while closing file descriptor");
					abort ();
				}
				
				/* Just writing a bit in the document to make it readable between each repetition */
				fprintf (stderr, "\n\nMicrolauncher execution...\n");
			}
			
			/* Getting executable's argument... */
			char **argv = Description_getExecArgv (desc);
			
			/* If argv == NULL, we have to create an argv table anyway... */
			if (argv == NULL)
			{
				char *argvNull[] = { fileName, NULL };
				execvp (fileName, argvNull);
			}
			/* Else, simply execute it with the argv table we got... */
			else
			{
				execvp (fileName, argv);
			}
			
			/* If we are there, it's because an error occured... */
			Log_output (-1, "(from child #%d) An error occured when executing \"%s\"\n", getppid(), fileName);
			perror("Error");
			abort (); /* For signal handling, we emit a SIGABRT */
		}
		default:
		{
			// Father
			int status;
			waitpid (pid, &status, 0);
			
			res = WEXITSTATUS (status);
			/* Child must end normally */
			if (WIFEXITED (status) == 0)
			{
				if (WIFSIGNALED (status))
				{
					 psignal (WTERMSIG (status), "Error: Input benchmark performed an error, exiting now...");
					 exit (EXIT_FAILURE);
				}
				Log_output (-1, "Benchmark ended non-normally, exiting now...\n");
				exit (EXIT_FAILURE);
			}
			break;
		}
	}
	
	popSignalHandler ();
	
	return res;
}

static inline double Benchmark_launchEvalFunction (evaluationFct fct, int isPrintingProcess, void *evalData) {
	if (fct != NULL && isPrintingProcess)
	{
		return fct (evalData);
	}
	return 0.;
}

/**
 * @brief Benchmark entry point for executables
 * @param desc the SDescription of this execution
 * @param EnableSync Enable/Disable the synchronisation (we are too fast in the overhead computation it leads to unstable beahavior)
 * @return the benchmark result (can be NULL)
 */
void
benchmark_exec ( BenchResult **res, SDescription *desc, int EnableSync, int overhead)
{
	int coarse_loop;
	uint64_t iterations = 0;
	
	int timeIsOk;
	int meta_repet = Description_getMetaRepetition (desc);
	int isRoot = Description_getRoot (desc);
	evaluationFct start;
	evaluationFct stop;
	void *evalData;
	int isPrintingProcess = Description_isPrintingProcess (desc);
	int isProcessEvalHandler = Description_isProcessEvalHandler (desc);
	int nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	double *dummyArray = Description_getDummyArrayAligned (desc);
	unsigned dummySize = Description_getDummySize (desc);
	verificationFctInit verifyInit = Description_getVerificationInitFct (desc);
	verificationFctDisplay verifyDisplay = Description_getVerificationDisplayFct (desc);
	verificationFctDestroy verifyDestroy = Description_getVerificationDestroyFct (desc);
	int i;
	FILE *fp;
	void *verifyContextData;
	
	pushSignalHandler (SignalHandler_launchingBenchmark);

	int *pipeSF = Description_getTubeSF (desc);
	int *pipeFS = Description_getTubeFS (desc);

	//Paranoid
	assert (res != NULL);
	
	/* Verifying library support */
	if (verifyInit != NULL && verifyDisplay != NULL && verifyDestroy != NULL)
	{
		/* Open and init verification stuff */
		char buf[STRBUF_MAXLEN];
		
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "test_exec.txt");
		fp = fopen (buf, "w");
		assert (fp != NULL);
		verifyContextData = verifyInit (desc);
		
		/* Launch with verification */
		execLauncher (desc, Description_getExecFileName ( desc ), 0, isPrintingProcess );
		
		/* Display and close verification context */
		verifyDisplay (verifyContextData, fp);
		verifyDestroy (verifyContextData);
		fclose (fp), fp = NULL;
	}
	
	// Meta loop that selects the lowest measure
	for (coarse_loop = 0; coarse_loop < meta_repet ; coarse_loop ++)
	{
		timeIsOk = 0; /* in the case of an overflow occur on the rdtsc register */
		Benchmark_printProgress (isPrintingProcess, coarse_loop, meta_repet, overhead, 0);
		
		if (EnableSync == 1)
		{
			barrierS (pipeSF, pipeFS);
		}
		
		/* Flush Caches */
		readDummyArray (dummyArray, dummySize);
		
		while (!timeIsOk)
		{
				if (isRoot)
				{
						iopl(3);
						asm volatile("cli");
				}
				
				/* --------------------------------------*/
				for (i = 0; i < nbEvalLibs; i++) /* Eval Start */
				{
					start = Description_getEvaluationStartFunction (desc, i);
					evalData = Description_getEvaluationData (desc, i);
					res[i]->initialTime = Benchmark_launchEvalFunction (start, isProcessEvalHandler, evalData);
				}
				
				if (overhead == 1)
				{
					char buf[STRBUF_MAXLEN];
					snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "example/empty/empty");
					iterations = execLauncher (desc, buf, 1, isPrintingProcess );
				}
				else
				{
					iterations = execLauncher (desc, Description_getExecFileName ( desc ), 0, isPrintingProcess );
				}
				
				for (i = nbEvalLibs-1; i >= 0; i--) /* Eval Stop */
				{
					stop = Description_getEvaluationStopFunction (desc, i);
					evalData = Description_getEvaluationData (desc, i);
					res[i]->finalTime = Benchmark_launchEvalFunction (stop, isProcessEvalHandler, evalData);
				}
				/* --------------------------------------*/

				if (isRoot)
				{
						asm volatile("sti");
				}
				
				/* Check : if one evaluation library has a negative value, then let's recompute results again */
				timeIsOk = 1;
				for (i = 0; i < nbEvalLibs; i++)
				{
					if ((res[i]->finalTime - res[i]->initialTime) < 0)
					{
						timeIsOk = 0;
						break;
					}
				}
		}

		for (i = 0; i < nbEvalLibs; i++)
		{
			res[i]->time[coarse_loop] = res[i]->finalTime - res[i]->initialTime;
			res[i]->iterations[coarse_loop] = iterations;
		}
	}
	
	Benchmark_printProgress (isPrintingProcess, meta_repet, meta_repet, overhead, 1);

	popSignalHandler ();
}

static inline void *Benchmark_loadEvaluationFunctions (SDescription *desc, const char *evallibName, unsigned idX) {
	char *startEval = "evaluationStart";
	char *stopEval = "evaluationStop";
	char *initEval = "evaluationInit";
	char *closeEval = "evaluationClose";
	char *flagOvhdName = "IS_OVERHEAD_RELEVANT";
	void *dl = NULL;
	evaluationFct startFct = NULL, stopFct = NULL;
	evaluationLogisticFctInit initFct = NULL;
	evaluationLogisticFctClose closeFct = NULL;
	evaluationFlag *overheadFlag = NULL;
	
	if (evallibName != NULL && startEval != NULL && stopEval != NULL && initEval != NULL && closeEval != NULL)
	{
		dl = dlopen (evallibName, RTLD_LAZY);
		
		if (dl == NULL)
		{
			Log_output (-1, "Error: Library name %s\n", dlerror ());
			return NULL;
		}

		//Clear any error
		dlerror ();

		startFct = dlsym (dl, startEval);
		if (startFct == NULL)
		{
			Log_output (-1, "Warning: Start function is not declared in Evaluation Library.\n");
		}
		stopFct = dlsym (dl, stopEval);
		if (stopFct == NULL)
		{
			Log_output (-1, "Warning: Stop function is not declared in Evaluation Library.\n");
		}
		initFct = dlsym (dl, initEval);
		if (initFct == NULL)
		{
			Log_output (-1, "Warning: Init function is not declared in Evaluation Library.\n");
		}
		closeFct = dlsym (dl, closeEval);
		if (closeFct == NULL)
		{
			Log_output (-1, "Warning: Close function is not declared in Evaluation Library.\n");
		}
		overheadFlag = dlsym(dl, flagOvhdName);

		//Set functions
		Description_setEvaluationStartFunction (desc, startFct, idX);
		Description_setEvaluationStopFunction (desc, stopFct, idX);
		Description_setEvaluationInitFunction (desc, initFct, idX);
		Description_setEvaluationCloseFunction (desc, closeFct, idX);
		Description_setEvaluationLibraryOverheadFlag (desc, overheadFlag, idX);
	}
	
	return dl;
}

static inline BenchResult *BenchResult_create (int meta_repet)
{
	BenchResult *br;
	
	br = malloc (sizeof (*br));
	assert (br != NULL);
	
	br->time = malloc ( meta_repet * sizeof (*br->time));
	br->iterations = malloc ( meta_repet * sizeof (*br->iterations));
	assert (br->iterations != NULL);
	assert (br->time != NULL);
	
	return br;
}

static inline void BenchResult_destroy (BenchResult *br)
{
	assert (br != NULL);
	
	free (br->time), br->time = NULL;
	free (br->iterations), br->iterations = NULL;
	free (br), br = NULL;
}

/**
 * @brief Benchmark launching executable
 * @param desc : The program parameters to be used
 * @param isPrintingProcess : Whether or not the current process is the one who has to handle output
 * @param currentExecRepet : Defines whether or not we have to add the current execution repetition of Microlauncher in the output filename
 * @return the benchmark execution status (typically EXIT_SUCCESS or EXIT_FAILURE)
 */
int Benchmark_Exec (SDescription *desc, int currentExecRepet)
{
	unsigned i;
	unsigned nbMetaRepetition = Description_getMetaRepetition (desc);
	BenchResult **overhead;
	BenchResult **res;
	double *overheadAvg;
	FILE *outputCsvFile  = NULL;
	void **dl_eval;
	evaluationLogisticFctInit init_timer;
	evaluationLogisticFctClose close_timer;
	void *timerInitRes = NULL;
	int timerCloseRes = 0;
	int threadsHasToBePinned = Description_isThreadPinningEnabled (desc);
	int isPrintingProcess = Description_isPrintingProcess (desc);
	int isProcessEvalHandler = Description_isProcessEvalHandler (desc);
	unsigned nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	unsigned evalLoop;
	void *evalData;
	int isRequestedToMakeFile = Description_getPromptOutputCsv (desc);

	pushSignalHandler (SignalHandler_child);
	
	Benchmark_printLaunchingTechnique (isPrintingProcess, EXEC_MODE);
	
	dl_eval = malloc ( nbEvalLibs * sizeof (*dl_eval));
	res = malloc ( nbEvalLibs * sizeof (*res));
	overhead = malloc ( nbEvalLibs * sizeof (*overhead));
	overheadAvg = malloc ( nbEvalLibs * sizeof (*overheadAvg));
	assert (overheadAvg != NULL);
	assert (overhead != NULL);
	assert (res != NULL);
	assert (dl_eval != NULL);
	
	/* Enable pthread pinning */
	if (threadsHasToBePinned)
	{
		char buf[STRBUF_MAXLEN];
		
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "Libraries/threadpinner/pinthread.so");
		
		if (setenv ("LD_PRELOAD", buf, 1) == -1) 
		{
			perror ("Error while setting LD_PRELOAD environment variable");
			return EXIT_FAILURE;
		}
	}
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		res[i] = BenchResult_create (nbMetaRepetition);
		overhead[i] = BenchResult_create (nbMetaRepetition);
		dl_eval[i] = Benchmark_loadEvaluationFunctions (desc, Description_getEvaluationLibraryName (desc, i), i);
		
		/* Init timer */
		init_timer = Description_getEvaluationInitFunction (desc, i);
		if (init_timer != NULL && isProcessEvalHandler)
		{
			timerInitRes = init_timer ();
			Description_setEvaluationData (desc, timerInitRes, i);
		}
		/* User seems to pass us some kind of allocated data ; we warn him that he has to
		free this himself */
		if (timerInitRes != NULL && isPrintingProcess)
		{
			Log_output (-1, "Warning: It looks like you have allocated some data structure in the %s evaluation library. You have to free this yourself in your close handler.\n",
					Description_getEvaluationLibraryName (desc, i));
		}
	}
	
	/* Allocate dummy array for cache flushes */
	Benchmark_makeDummyArray (desc);
	
	/* CSV output handling */
	if (isProcessEvalHandler && isRequestedToMakeFile)
	{
		outputCsvFile = Benchmark_createOutputFile (desc, currentExecRepet, -1);
		if (outputCsvFile == NULL)
		{
			return EXIT_FAILURE;
		}
	   	BenchmarkExec_initCsv (desc, outputCsvFile);
	}

	/* Overhead computation */
	benchmark_exec (overhead, desc, 1, 1);
	
	/* Real computation */
	benchmark_exec (res, desc, 1, 0);
	
	if (isProcessEvalHandler && isRequestedToMakeFile)
	{
		/* Computing the overhead average for each evaluation library */
		for (evalLoop = 0; evalLoop < nbEvalLibs; evalLoop++)
		{
			overheadAvg[evalLoop] = 0;
			
			/* this lib requested no overhead computation */
			if (!Description_getEvaluationLibraryOverheadFlag(desc, evalLoop))
			{
				continue;
			}
			
			for ( i = 0 ; i < nbMetaRepetition ; i++ )
			{
				overheadAvg[evalLoop] += overhead[evalLoop]->time[i];
			}
			overheadAvg[evalLoop] /= nbMetaRepetition;
		}
		
		/* Computing all values we wish to use */
		for (i = 0 ; i < nbMetaRepetition ; i++)
		{
			int problem = NO_ERROR;
			for (evalLoop = 0; evalLoop < nbEvalLibs; evalLoop++)
			{
				res[evalLoop]->time[i] -= overheadAvg[evalLoop];

				if (res[evalLoop]->time[i] < 0)
				{
					problem = OVERHEAD_TOO_HIGH;
				}
			}
			
			/* Print every eval lib result in the CSV */
			BenchmarkExec_printCsv (res, nbEvalLibs, i, outputCsvFile, problem);
		}
		
		Benchmark_printDataSavingProgress (isPrintingProcess, nbMetaRepetition, nbMetaRepetition, 100, 0, 1);
		fclose (outputCsvFile), outputCsvFile = NULL;
	}
	
	/* Close Timer */
	for (i = 0; i < nbEvalLibs; i++)
	{
		close_timer = Description_getEvaluationCloseFunction (desc, i);
		evalData = Description_getEvaluationData (desc, i);
		if (close_timer != NULL && isProcessEvalHandler)
		{
			timerCloseRes = close_timer (evalData);
		}
		assert (timerCloseRes == EXIT_SUCCESS);
	}
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		BenchResult_destroy (overhead[i]), overhead[i] = NULL;
		BenchResult_destroy (res[i]), res[i] = NULL;
	}
	free (res), res = NULL;
	free (overhead), overhead = NULL;
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		if (dl_eval[i] != NULL)
		{
			dlclose (dl_eval[i]), dl_eval[i] = NULL;
		}
	}
	free (dl_eval), dl_eval = NULL;
	free (overheadAvg), overheadAvg = NULL;
	
	/* Disable pthread pinning */
	if (threadsHasToBePinned)
	{
		if (setenv ("LD_PRELOAD", "", 1) == -1)
		{
			perror ("Error while unsetting LD_PRELOAD environment variable");
			return EXIT_FAILURE;
		}
	}
	
	popSignalHandler ();

	return EXIT_SUCCESS;
}

void BenchmarkExec_initCsv (SDescription *desc, FILE *stream) {
	unsigned nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	unsigned i;
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		fprintf (stream, "\"Eval '%s'\",", Description_getEvaluationLibraryName (desc, i));
	}
	fprintf (stream, "\n");
}

void BenchmarkExec_printCsv (BenchResult **res, unsigned nbEvalLibs, unsigned currentMetaRepet, FILE *stream, int problem)
{
	unsigned i;
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		fprintf (stream, "%0.6f,", res[i]->time[currentMetaRepet]);
	}
	
	/* If an error occured with the benchmark result */
	if (problem != NO_ERROR)
	{
		fprintf (stream, "%d,", problem);
	}
	fprintf(stream, "\n");
	fflush(stream);
}




