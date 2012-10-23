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

/**
@mainpage Micro-benchmark launcher executes micro-benchmark programs
@version 1.00
@author Jean Christophe Beyler
@date 04/11/2010
*/

#define VERSION "Version 2.4.7 ("__DATE__")"

#define AFFINITY_AVAILABLE 0

  
/*============= INCLUDES ===================================================*/
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "BenchDescriptor.h"
#include "Benchmark.h"
#include "BenchmarkExec.h"
#include "Config.h"
#include "Defines.h"
#include "Description.h"
#include "Dflush.h"
#include "Log.h"
#include "Options.h"
#include "Pipes.h"
#include "Resume.h"
#include "Signal.h"
#include "SleepTight.h"
#include "Toolkit.h"

/**
 * @brief Main function
 * @param argc Number of arguments
 * @param **argv The arguments
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
int 
main (int argc, char ** argv)
{
    unsigned i;
    int *process_pinning = NULL;
	int status;
	pid_t pid;
	unsigned nbprocess;
	SPipe *pipes;
	int res;
	int isOpenMP = 0;
	int iterationsDoneAlready = 0;
	unsigned currentExecRepet;
	unsigned execRepets, kernelId, nbKernels;
	
	printf("*************************************************************************************************\n");
	printf("* |\\   /|   '    ____  ____   ____          ____                    ____           ____  ____\t*\n");
	printf("* | \\ / |   |   |     |____| |    | |      |____|  |    |  | \\  |  |      |____|  |____ |____|\t*\n");
	printf("* |  '  |   |   |____ |   \\  |____| |____  |    |  |____|  |  \\ |  |____  |    |  |____ |   \\\t*\n");
	printf("*\t\t\t\t\t\t\t\t\t\t\t\t*\n");
	printf("*************************************************************************************************\n");

    /* Handle options */
    if (Option_handleHelpRequest (argc, argv) == 0)
    {
        return EXIT_SUCCESS;
	}

    /* Before anything, handle signal */
    pushSignalHandler (SignalHandler_microlauncher);
    
    fprintf (stderr, "ML Start %d\n", getpid ());
    Log_setOutput (stderr);
    Log_setVerbosity (8);
	
	/* Then, we need to generate the program data description */
    SDescription *desc = Description_create ();
    /* Next set Log to stderr */

    // Handle options
    Option_handleCommandLine (desc, argc, argv);
    
    /* Now that we have handled user customed values, let's set unset values to our defaults */
    Description_setDefaults (desc);
	
    // Arguments verification
    if (Description_AssertValues (desc) == -1) {
    	Log_output (-1, "Error: Exiting Microlauncher now...\n");
    	exit (EXIT_FAILURE);
    }
    
    // Warn the user
    Description_ShowWarnings (desc);

    // If the file PATH_TO_KILL exists, cancel the program (sleep_tight.h)
    escape_from_scary_killers ();
    
    /* If current architecture isn't good for us */
    if (sizeof (unsigned long long) != 8)
    {
        Log_output (-1, "Error: Unsigned long long size != 64 bit ! Exiting...\n");
        exit (EXIT_FAILURE);
    }

	/* Prepare the sleep tight signals */
	if (sleep_tight_preparesignals() != 0)
	{
		Log_output (-1, "Error! Could not prepare Sleep Tight's signals! Aborting.\n");
		exit(EXIT_FAILURE);
	}

	/* Now that the desc values have been all handled, we save them for possible resuming request */
	resumeSave (desc);

	/* Allocate data for the pipes and process pinning */
	nbprocess = Description_getNbProcess (desc);
	pipes = malloc (sizeof (*pipes) * nbprocess);
	assert (pipes != NULL);
	memset (pipes, 0, sizeof (*pipes) * nbprocess);
	
	/* Allocate process pinning table only if we're not in OpenMP mode as this mode doesn't pin CPUs */
	isOpenMP = Description_getOmpPath (desc) != NULL;
	if (!isOpenMP)
	{
		process_pinning = malloc (sizeof (*process_pinning) * nbprocess);
		assert(process_pinning != NULL);
		memset (process_pinning, 0, sizeof (*process_pinning) * nbprocess);
		/* Process pinning initialization */
		for (i = 0; i < nbprocess ; i++)
		{
            Log_output (-1, "Getting pinning %d: %d\n", i, Description_getPinningAt (desc, i));
            process_pinning[i] = Description_getPinningAt (desc, i);
		}
	}
	
	/* Get the father aware of the number of experiments to be done by its children */
	Description_setExperimentNumber (desc, getExperimentNumber (desc));
	
	execRepets = Description_getExecuteRepets (desc);
	if (Description_getExecFileName (desc) != NULL)
	{
		nbKernels = 1;
	}
	else
	{
		nbKernels = Description_getKernelFileNamesTabSize (desc);
	}
	
	/* Multiple kernels handling */
	for (kernelId = 0; kernelId < nbKernels; kernelId++)
	{
		if (Description_getExecFileName (desc) == NULL)
		{
			/* Setting the current kernel name to the current id we're running */
			Description_setKernelCurrentFileId (desc, kernelId);
			Log_output (-1, "Current Kernel Execution : %s\n", Description_getKernelFileName (desc));
		}
		
		/* Re-init to last known currentExecRepet value if we're resuming */
		if (resumeInitCounter (&currentExecRepet, desc->temp_values.current_execute_repet))
		{
			Log_output (-1, "Note: Resuming to last --executerepet value : %d\n", currentExecRepet);
		}
		else
		{
			currentExecRepet = 0;
		}
		
		/* Repeats Microlauncher --executerepet (argument) times */
		for (; currentExecRepet < execRepets; currentExecRepet++)
		{
			if (resumeIsResuming ())
			{
				resumeLoadCounters (desc);
			}
		
			/* If more than 1 execution repeat, get the user aware of current iteration */
			if (execRepets > 1)
			{
				Log_output (-1, "\n\nMicrolaunch Repeat #%d\n", currentExecRepet+1);
			}
		
			/* Input file compilation */
			compileInputFile (desc);
		
			/* Processes launch */
            fprintf (stderr, "ML: %d -> Creating %d\n", getpid (), nbprocess);
			for (i = 0; i < nbprocess ; i++)
			{
				Pipe_generate (&pipes[i]);
				pid = fork ();

				if (pid == 0)
				{
                    fprintf (stderr, "ML: %d: Creating son: %d\n", getppid (), getpid ());
                    //Start formatting strings: for the moment just exec arguments, exec name, and exec path
                    Description_reformatStrings (desc, i);

                    //If we must change directory
                    char *res = Description_getExecPath (desc);

                    if (res != NULL)
                    {
                        int chroot_res = chdir (res);
                        if (chroot_res != 0)
                        {
                            perror ("");
                            exit (EXIT_FAILURE);
                        }
                    }

					/* Push the signal handler : We're now in the child function */
					pushSignalHandler (SignalHandler_child);

					/* Defines the process Id */
					Description_setProcessId (desc, i);
				
					/* By default, we enable the printing and eval handling stuff for the first child */
					if(i == 0 || Description_getAllPrintOut (desc) != 0)
					{
					    Description_printingProcessEnable (desc);
						Description_processEvalHandlerEnable (desc);
					}

					/* If we have allprocess-output argument, then every process compute its counters */
					if (Description_isAllProcessOutputEnabled (desc))
					{
						Description_processEvalHandlerEnable (desc);
					}
			
					/* If we're not in OpenMP mode and we do pin the threads, we pin the processes on the machine's cores */
					if (isOpenMP == 0 && Description_isThreadPinningEnabled (desc) != 0)
					{
						if (!Description_isCpuPinDefined (desc))
						{
							Description_setCPUDest (desc, process_pinning[i]);
						}

						//Pin CPU
						if (Description_pinCPU (desc) == -1)
						{
							abort ();
						}
					}
			
					/* We save the pipes in the description for barrierS future calls (see benchmark.c) */
					Description_setTubeSF (desc,pipes[i].sf);
					Description_setTubeFS (desc,pipes[i].fs);

					//Close the unused sides of pipes
					Pipe_closeChildUnusedSide ( &pipes[i] );
			
					/* Are we launching an executable or do we want to launch a function in
						a dynamic library ? */
					switch (Description_getSourceType (desc))
					{
						case EXECUTABLE_FILE: /* EXEC MODE */
						{
							Pipe_closeChildUnusedSide ( &pipes[i] );
							status = Benchmark_Exec (desc, currentExecRepet);
							if (status != EXIT_SUCCESS)
							{
								abort ();
							}
			
							break;
						}
						case LIBRARY_FILE:
						case SOURCE_FILE:
						case ASSEMBLY_FILE:
						case OBJECT_FILE: /* KERNEL MODE */
						{
							status = Benchmark_Wrapper (desc, currentExecRepet);
							if (status != EXIT_SUCCESS)
							{
								abort ();
							}
					
							break;
						}
						default:
							Log_output (-1, "Error: Unknown File source type, cannot launch benchmark, code %d\n", Description_getSourceType (desc));
							exit (EXIT_FAILURE);
					}
					//We have finished, close the rest
					Pipe_closeChildUsedSide ( &pipes[i] );

					// Take away the signal handler (child)
					popSignalHandler ();
			
					/* Son free stuff */
					if (!isOpenMP)
					{
						free(process_pinning), process_pinning = NULL;
					}
					free(pipes), pipes = NULL;

					int isPrintingProcess = Description_isPrintingProcess (desc);
					//Destroy description
					Description_destroy (desc), desc = NULL;

					if (isPrintingProcess)
					{
						Log_output (-1, "Child process exiting now...\n");
					}
				
					// Take away the signal handler (father)
					popSignalHandler ();
					exit (EXIT_SUCCESS);
				}
				else if(pid < 0)
				{
					perror("fork");
					exit(EXIT_FAILURE);
				}
				else
				{
					/* Close non-used side of each pipe */
					Pipe_closeFatherUnusedSide ( &pipes[i] );
				}
			}
	
			if (resumeIsResuming ())
			{
				iterationsDoneAlready = desc->temp_values.curruns;
			}
			else
			{
				iterationsDoneAlready = 0;
			}

			/* Setting up the father Barrier */
			unsigned max = benchmarkIterationsNumber (desc, iterationsDoneAlready);
			for (i = 0 ; i < max ; i++)
			{
				barrierF (pipes, nbprocess);
			}

			/* Close the rest of the pipes */
			for (i = 0 ; i < nbprocess ; i++)
			{
				Pipe_closeFatherUsedSide ( &pipes[i] );
			}
	
			/* Child processes wait */
			for (i = 0 ; i < nbprocess ; i++)
			{
				waitpid (-1, &status, 0);
				res = WEXITSTATUS(status);
				if(res != EXIT_SUCCESS)
				{
					Log_output (-1, "Child exited with status %d, an error occured.\n", res);
				}
				if (WIFEXITED(status) == 0)
				{
					char buf[512];
			
					snprintf (buf, 512, "Error: Child %d received a signal ", i);
					psignal (WTERMSIG (status), buf);
				}
			}
			desc->temp_values.current_execute_repet++;
			/*	Disabling resume in the innermost loop of the father process
				because resuming has to be done just for the first iteration of each process loops */
			resumeDisableResuming ();
		}
	}
	
	/* Job is done, let's create a file to tell the user so */
	resumeSignalJobDone (desc);
	
	/* Free data */
	free (pipes), pipes = NULL;
	if (!isOpenMP)
	{
		free (process_pinning), process_pinning = NULL;
	}
	
	if (Description_isSummaryEnabled (desc))
	{
		if (generateSummary (desc) != -1) {
			Log_output (-1, "Info: A summary.xls file has been created in the %s folder.\n", Description_getOutputPath (desc));
		} else {
			Log_output (-1, "Error: An error occured while creating summary file.\n");
		}
	}
	
	Description_destroy ( desc );
	
	Log_output(-1, "Father process exiting now...\n");
	
	//Take away the signal handler
	popSignalHandler ();

	return EXIT_SUCCESS;
}

