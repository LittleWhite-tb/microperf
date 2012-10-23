/*
Copyright (C) 2011 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the homape that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#define _GNU_SOURCE
#include <sched.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "Benchmark.h"
#include "Config.h"
#include "Description.h"
#include "Log.h"
#include "Toolkit.h"

//Static function
static char *reformatString (char *orig, int procId);
static char *reformatUsingBash (char *tmp);

SDescription *Description_create (void)
{	
	SDescription *res = malloc (sizeof (*res));
	assert (res);
	memset (res, 0, sizeof (*res));
	

	//Set initial values (for DEFAULT values, see Description_setDefaults ())
	Description_nbSizesDisable (res); 	
	Description_setNbVectors (res, DEFAULT_NB_VECTORS);
	
	Description_setStartVectorSize (res, DEFAULT_ARRAY_SIZE);
	Description_setEndVectorSize (res, DEFAULT_ARRAY_SIZE);
	Description_setVectorSizeStep (res, DEFAULT_STEP_VECTOR);
	Description_setVectorElementSize (res, DEFAULT_ELEM_SIZE);
	Description_setCPUDest (res, DEFAULT_CPU_DEST);
	Description_iterationCountDisable (res);
	Description_setIterationCount (res, DEFAULT_ITERATION_COUNT);
	Description_setConfigFile (res, NULL);
	Description_setNbPagesForCollision (res, DEFAULT_NBPAGESFORCOLLISION);
	Description_setRepetition (res, DEFAULT_REPETITION);
	Description_setMetaRepetition (res, DEFAULT_META_REPETITION);
	Description_setExecuteRepets (res, DEFAULT_EXEC_REPETITION);
	Description_setDummySize (res, DEFAULT_DUMMYSIZE);
	Description_setMaxStride (res, DEFAULT_MAX_STRIDE);
	Description_setLogOutput (res, stderr);
	Description_setBaseName (res, NULL);				//OK
	Description_setPromptOutputCsv (res, DEFAULT_PROMPT_OUTPUT);
	Description_setOmpPath (res, NULL);
	Description_setKernelFileName (res, NULL);
	Description_setOutputPath (res, NULL);
	Description_setNbProcessorsAvailable (res, DEFAULT_NBPROC_AVAILABLE);
	Description_setRoot (res, DEFAULT_USER_ROOT);
	Description_setExecArgv (res, NULL);
	Description_setExecArgc (res, DEFAULT_EXEC_ARGC);
	Description_setNbProcess (res, DEFAULT_NB_PROCESS);
	Description_setSuppressOutput (res, DEFAULT_SUPPRESS_OUTPUT);
	Description_setOutputFileStream (res, NULL);
	Description_setDynamicFunctionName (res, NULL);
	Description_setKernelInitFunction (res, NULL);
	Description_setKernelInitFunctionName (res, NULL);
	Description_setSourceType (res, UNKNOWN_FILE); // File type is unknown for the moment
	Description_setPageSize (res, DEFAULT_PAGESIZE);
	Description_setDynAllocLib (res, NULL);
	Description_setResuming (res, DEFAULT_RESUMING_VALUE); // Resuming system
	Description_setNumber_of_resumes (res, DEFAULT_RESUME_NB);
	Description_setResumeId (res, DEFAULT_RESUME_ID);
	Description_pinThreadEnable (res);
	Description_allProcessOutputDisable (res);
	Description_disableSummary (res);
	Description_setVerificationLibraryName (res, NULL);
	Description_setVerificationContextData (res, NULL);
	Description_setVerificationInitFct (res, NULL);
	Description_setVerificationDisplayFct (res, NULL);
	Description_setVerificationDestroyFct (res, NULL);
    	Description_setAllPrintOut (res, 0);
	Description_enableEvalStack (res);
	return res;
}

void Description_setDefaults (SDescription *desc) {
	assert (desc != NULL);
	
	if (Description_getStartVectorSize (desc) == DEFAULT_ARRAY_SIZE)
	{
		Log_output (5, "Info: Defining start-vector size value : 2500000\n");
		Description_setStartVectorSize (desc, 2500000);
	}
	
	/* If startvector is defined and not endvector, set it to startvector value */
	if 	(Description_getEndVectorSize (desc) == DEFAULT_ARRAY_SIZE)
	{
		Log_output (5, "Info: Defining end-vector size value : 2500000\n");
		Description_setEndVectorSize (desc, Description_getStartVectorSize (desc));
	}
	
	if (Description_getMetaRepetition (desc) == DEFAULT_META_REPETITION)
	{
		Log_output (5, "Info: Defining meta-repetition value : 40\n");
		Description_setMetaRepetition (desc, 40);
	}
	
	if (Description_getMetaRepetition (desc) == DEFAULT_META_REPETITION && Description_getExecuteRepets (desc) != DEFAULT_EXEC_REPETITION)
	{
		Log_output (5, "Info: Defining meta-repetition value : 2500000\n");
		Description_setMetaRepetition (desc, 1);
	}
	
	if (Description_getCPUDest (desc) == DEFAULT_CPU_DEST)
	{
		Log_output (5, "Info: Defining CPU destination : 0\n");
		Description_setCPUDest (desc, 0);
	}
	
	if (Description_getRepetition (desc) == DEFAULT_REPETITION)
	{
		Log_output (5, "Info: Defining repetition value : 20\n");
		Description_setRepetition (desc, 20);
	}
	
	if (Description_getExecuteRepets (desc) == DEFAULT_EXEC_REPETITION)
	{
		Log_output (5, "Info: Defining execute-repetition value : 1\n");
		Description_setExecuteRepets (desc, 1);
	}
	
	if (Description_getVectorSizeStep (desc) == DEFAULT_STEP_VECTOR)
	{
		Log_output (5, "Info: Defining vector size step value : 1\n");
		Description_setVectorSizeStep (desc, 1);
	}
	
	if (Description_getVectorElementSize (desc) == DEFAULT_ELEM_SIZE)
	{
		Log_output (5, "Info: Defining vector element size value : %u\n", sizeof (float));
		Description_setVectorElementSize (desc, sizeof (float));
	}
	
	if (Description_getVectorSpacing (desc) == DEFAULT_VECTOR_SPACING)
	{
		Log_output (5, "Info: Defining vector spacing value : 496\n");
		Description_setVectorSpacing (desc, 496);
	}
	
	if (Description_getNbPagesForCollision (desc) == DEFAULT_NBPAGESFORCOLLISION)
	{
		Log_output (5, "Info: Defining nb pages for collision value : 4\n");
		Description_setNbPagesForCollision (desc, 4);
	}
	
	if (Description_getDummySize (desc) == DEFAULT_DUMMYSIZE)
	{
		Log_output (5, "Info: Defining dummy size value : 3000000\n");
		Description_setDummySize (desc, 3000000); /* We set this to 3M doubles, it should handle most of the current architectures */
	}
	
	if (Description_getMaxStride (desc) == DEFAULT_MAX_STRIDE)
	{
		Log_output (5, "Info: Defining max stride value : 1\n");
		Description_setMaxStride (desc, 1);
	}
	
	if (Description_getBaseName (desc) == NULL)
	{
		Log_output (5, "Info: Defining base name value : default\n");
		Description_setBaseName (desc, "default");
	}
	
	if (Description_getPromptOutputCsv (desc) == DEFAULT_PROMPT_OUTPUT)
	{
		Log_output (5, "Info: Defining prompt output csv value : 1\n");
		Description_setPromptOutputCsv (desc, 1);
	}
	
	if (Description_getNbEvaluationLibrairies (desc) == 0)
	{
		char buf[STRBUF_MAXLEN];
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "Libraries/timer/timer.so");
		
		Description_setNbEvaluationLibrairies (desc, 1);
		Log_output (5, "Info: Defining evaluation library : timer/timer.so\n");
		Description_setEvaluationLibraryName (desc, buf, 0);
		
		if (desc->kernelFileName == NULL)
		{
			Description_setInfoDisplayed (desc, "raw", 0);
			Log_output (5, "Info: Defining info displayed value : RAW_NUMBERS\n");
		}
		else
		{
			Description_setInfoDisplayed (desc, "iteration", 0);
			Log_output (5, "Info: Defining info displayed value : ITERATION_COST\n");
		}
	} else if (Description_getEvaluationLibraryName (desc, 0) == NULL)
	{
		char buf[STRBUF_MAXLEN];
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "Libraries/timer/timer.so");
		Description_setEvaluationLibraryName (desc, buf, 0);
	}
	
	if (Description_getOutputPath (desc) == NULL)
	{
		Log_output (5, "Info: Defining output path value : output\n");
		Description_setOutputPath (desc, "output");
	}
	
	if (Description_getNbProcessorsAvailable (desc) == DEFAULT_NBPROC_AVAILABLE)
	{
		Log_output (5, "Info: Defining output path value : %u\n", getNbProcessorsAvailable ());
		Description_setNbProcessorsAvailable (desc, getNbProcessorsAvailable ());
	}
	
	if (Description_getExecArgc (desc) == DEFAULT_EXEC_ARGC)
	{
		Log_output (5, "Info: Defining exec argc value : 0\n");
		Description_setExecArgc (desc, 0);
	}
	
	if (Description_getNbProcess (desc) == DEFAULT_NB_PROCESS)
	{
		Log_output (5, "Info: Defining nbProcess value : 1\n");
		Description_setNbProcess (desc, 1);
	}
	
	if (Description_getSuppressOutput (desc) == DEFAULT_SUPPRESS_OUTPUT)
	{
		Log_output (5, "Info: Defining suppress output value : 0\n");
		Description_setSuppressOutput (desc, 0);
	}
	
	if (Description_getRoot (desc) == DEFAULT_USER_ROOT)
	{
		Log_output (5, "Info: Defining root value : %d\n", isUserRoot ());
		Description_setRoot (desc, isUserRoot ());
	}
	
	if (Description_getDynamicFunctionName (desc) == NULL)
	{
		Log_output (5, "Info: Defining Dynamic function name value : entryPoint\n");
		Description_setDynamicFunctionName (desc, "entryPoint");
	}
	
	if (Description_getPageSize (desc) == DEFAULT_PAGESIZE)
	{
		Log_output (5, "Info: Defining page size value : %d\n", getpagesize () * Description_getNbPagesForCollision (desc));
		Description_setPageSize (desc, getpagesize () * Description_getNbPagesForCollision (desc));
	}
	
	if (Description_getDynAllocLib (desc) == NULL)
	{
		char buf[STRBUF_MAXLEN];
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "Libraries/allocator/dedicated_arrays/dedicated_arrays.so");
		Log_output (5, "Info: Defining dynamic allocation library value : Libraries/allocator/dedicated_arrays/dedicated_arrays.so\n");
		Description_setDynAllocLib (desc, buf);
	}
	
	if (Description_getResuming (desc) == DEFAULT_RESUMING_VALUE)
	{
		Log_output (5, "Info: Defining resuming value : 0\n");
		Description_setResuming (desc, 0);
	}
	
	if (Description_getNumber_of_resumes (desc) == DEFAULT_RESUME_NB)
	{
		Log_output (5, "Info: Defining number of resumes value : 0\n");
		Description_setNumber_of_resumes (desc, 0);
	}
	
	if (Description_getResumeId (desc) == DEFAULT_RESUME_ID)
	{
		Log_output (5, "Info: Defining resume id value : 0\n");
		Description_setResumeId (desc, 0);
	}
}

void Description_destroy (SDescription *desc)
{
	unsigned i, nbVectors = Description_getNbVectors (desc);
	unsigned nbKernels = Description_getKernelFileNamesTabSize (desc);
	unsigned nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	if (desc != NULL)
	{
		free (desc->baseName), desc->baseName = NULL;
		free (desc->dummyArrayNonAligned), desc->dummyArrayNonAligned = NULL;

		if (desc->dynlibDelete != 0)
		{
			remove (desc->dynlibName);
		}

		free (desc->dynlibName), desc->dynlibName = NULL;
		free (desc->dynlibFunc), desc->dynlibFunc = NULL;
		free (desc->kernelFileName), desc->kernelFileName = NULL;
		free (desc->ompPath), desc->ompPath = NULL;
		free (desc->execFileName), desc->execFileName = NULL;
		free (desc->configFileName), desc->configFileName = NULL;
		free (desc->outputPath), desc->outputPath = NULL;
		free (desc->outputFileStream), desc->outputFileStream = NULL;
		free (desc->kernelInitFunctionName), desc->kernelInitFunctionName = NULL;
		free (desc->vectorSizes), desc->vectorSizes = NULL;
		free (desc->outputFileName), desc->outputFileName = NULL;
        free (desc->execPath), desc->execPath = NULL;
		
		/* Vector Descriptor free */
		for (i = 0; i < nbVectors; i++)
		{
			free (desc->vectorDescriptor[i]), desc->vectorDescriptor[i] = NULL;
		}
		free (desc->vectorDescriptor), desc->vectorDescriptor = NULL;
	
		/* Exec args free */
		freeArgs (desc->execArgc, desc->execArgv);
		
		/* Alignments settings */
		free(desc->temp_values.alignments), desc->temp_values.alignments = NULL;
		
		/* Dynamic allocation library */
		free (desc->dynAllocLib), desc->dynAllocLib = NULL;
		
		for (i = 0; i < nbKernels; i++)
		{
			free (desc->kernelFileNames[i]), desc->kernelFileNames[i] = NULL;
		}
		free (desc->kernelFileNames), desc->kernelFileNames = NULL;
		free (desc->verificationLibraryName), desc->verificationLibraryName = NULL;
		
		for (i = 0; i < nbEvalLibs; i++)
		{
			free (desc->evaluationLibraryName[i]), desc->evaluationLibraryName[i] = NULL;
		}
		free (desc->evaluationLibraryName), desc->evaluationLibraryName = NULL;
		free (desc->evaluationInit), desc->evaluationInit = NULL;
		free (desc->evaluationClose), desc->evaluationClose = NULL;
		free (desc->evaluationStart), desc->evaluationStart = NULL;
		free (desc->evaluationStop), desc->evaluationStop = NULL;
		free (desc->evaluationOverheadFlags), desc->evaluationOverheadFlags = NULL;
		free (desc->evalData), desc->evalData = NULL;
		free (desc->infoDisplayed), desc->infoDisplayed = NULL;
		
		free (desc->pinning), desc->pinning = NULL;
		
		free (desc), desc = NULL;
	}
}

void Description_ShowWarnings (SDescription *desc)
{
	unsigned value;
	
	if (Description_isIterationCountEnabled (desc))
	{
		Log_output (-1, "Warning: You defined a constant iteration count yourself.\n");
	}
	
	if (Description_getRoot (desc))
    {
        Log_output (-1, "Warning: Program is masking the interruptions\n");
    }
    else
    {
        Log_output (-1, "Warning: not able to mask the interruptions\n");
    }
    
    if (Description_getOmpPath (desc) != NULL)
    {
    	Log_output (-1, "Warning: No process pinning because we are in OpenMP mode\n");
    }
    
	if(Description_getPromptOutputCsv (desc) == 0)
	{
		Log_output (-1, "Warning: There will be no output CSV file\n");
	}
	
	value = Description_getKernelFileNamesTabSize (desc);
	if (value > 1)
	{
		Log_output (-1, "Warning: You're launching %d kernels\n", value);
	}
	
	/* This means the user wants several input files and has defined a basename
		This basename is going to be erased, so we have to warn him */
	if (Description_getKernelFileNamesTabSize (desc) > 1
		&& strncmp (desc->baseName, "default", STRBUF_MAXLEN) != 0)
	{
		Log_output (-1, "Warning: You defined a basename, but because you requested several input files, this basename will not be taken into account.\n", value);
	}
}

void Description_setExecuteRepets (SDescription *desc, int value) {
	assert (desc != NULL);
	desc->executeRepet = value;
}

int Description_getExecuteRepets (SDescription *desc) {
	assert (desc != NULL);
	return desc->executeRepet;
}

void Description_setStartVectorSize (SDescription *desc, int value)
{
	assert (desc);
	desc->startVectorSize = value;
	
	if (!Description_isNbSizeDefined (desc))
	{
		Description_setVectorSizes (desc, value);
	}
}

int Description_getStartVectorSize (SDescription *desc)
{
	assert (desc);
	return desc->startVectorSize;
}

void Description_iterationCountDisable (SDescription *desc)
{
	assert (desc);
	desc->iterationCount_Enabled = 0;
}

void Description_iterationCountEnable (SDescription *desc)
{
	assert (desc);
	desc->iterationCount_Enabled = 1;
}

int Description_isIterationCountEnabled (SDescription *desc)
{
	assert (desc);
	return desc->iterationCount_Enabled;
}

void Description_setIterationCount (SDescription *desc, int value)
{
	assert (desc);
	desc->iterationCount = value;
}

int Description_getIterationCount (SDescription *desc)
{
	assert (desc != NULL);
	return desc->iterationCount;
}

void Description_setEndVectorSize (SDescription *desc, int value)
{
	assert (desc);
	desc->endVectorSize = value;
}

int Description_getEndVectorSize (SDescription *desc)
{
	assert (desc);
	return desc->endVectorSize;
}

void Description_setVectorSizeStep (SDescription *desc, int value)
{
	assert (desc);
	desc->vectorSizeStep = value;
}

int Description_getVectorSizeStep (SDescription *desc)
{
	assert (desc);
	return desc->vectorSizeStep;
}

void Description_setExperimentNumber (SDescription *desc, int value)
{
	assert (desc);
	desc->experimentNumber = value;
}

int Description_getExperimentNumber (SDescription *desc)
{
	assert (desc);
	return desc->experimentNumber;
}

void Description_setRoot (SDescription *desc, int value)
{
	assert (desc);
	desc->isRoot = value;
}

int Description_getRoot (SDescription *desc)
{
	assert (desc);
	int res = desc->isRoot;
	return res;
}

void Description_setPromptOutputCsv(SDescription *desc, int value)
{
	assert(desc);
	desc->promptOutputCsv = value;
}

int Description_getPromptOutputCsv(SDescription *desc)
{
	assert(desc);
	return desc->promptOutputCsv;
}

void Description_setInfoDisplayed (SDescription *desc, char *value, unsigned idX)
{
	assert (desc != NULL && value != NULL);
	assert (idX < desc->nbEvalLibs);
	
	if (strncmp ("iteration", value, STRBUF_MAXLEN) == 0)
	{
		desc->infoDisplayed[idX] = ITERATION_COST;
	}
	else if (strncmp ("function", value, STRBUF_MAXLEN) == 0)
	{
		desc->infoDisplayed[idX] = FUNCTION_COST;
	}
	else if (strncmp ("raw", value, STRBUF_MAXLEN) == 0)
	{
		desc->infoDisplayed[idX] = RAW_NUMBERS;
	}
	else
	{
		desc->infoDisplayed[idX] = RAW_NUMBERS;
	}
}

int Description_getInfoDisplayed (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->infoDisplayed[idX];
}

int Description_getMaxStride (SDescription *desc)
{
	assert (desc);
	int res = desc->maxStride;
	return res;
}

void Description_setMaxStride (SDescription *desc, int value)
{
	assert (desc);
	desc->maxStride = value;
}

int Description_getDummySize (SDescription *desc)
{
	assert (desc);
	return desc->dummySize;
}

void Description_setDummySize (SDescription *desc, int value)
{
	assert (desc);
	desc->dummySize = value;
}

double *Description_getDummyArrayAligned (SDescription *desc)
{
	assert (desc);
	return desc->dummyArrayAligned;
}

void Description_setDummyArray (SDescription *desc, double* aligned, double* non_aligned)
{
	assert (desc);

	//Free array 
	free (desc->dummyArrayNonAligned), desc->dummyArrayNonAligned = NULL;

	desc->dummyArrayAligned = aligned;
	desc->dummyArrayNonAligned = non_aligned;
}

void Description_setVectorSpacing (SDescription *desc, int value)
{
	assert (desc);
	if (value <= 0)
	{
		Log_output (-1, "Error: Vectorspacing argument value must be greater than 0 (given value : %d).\n", value);
		exit (EXIT_FAILURE);
	}
	desc->vectorSpacing = value;
}

int Description_getVectorSpacing (SDescription *desc)
{
	assert (desc);
	int res = desc->vectorSpacing;
	return res;
}

int Description_getNbProcess (SDescription *desc)
{
	assert(desc);
	int res = desc->nbprocess;
	return res;
}

void Description_setNbProcess (SDescription *desc, int val)
{
    int i;

	assert (desc);
    desc->nbprocess = val;

    //Free it
	free (desc->pinning), desc->pinning = NULL;

    if (val > 0)
    {
        /* Customed vectors sizes array */
        desc->pinning = malloc (val * sizeof (*desc->pinning));
        assert (desc->pinning != NULL);
        memset (desc->pinning, 0, val * sizeof (*desc->pinning));

        for (i = 0; i < val; i++)
        {
            desc->pinning[i] = i;
        }
    }
}

int Description_getNbProcessorsAvailable (SDescription *desc)
{
	assert(desc);
	return desc->nbProcessorsAvailable;
}

void Description_setNbProcessorsAvailable (SDescription *desc, int value)
{
	assert(desc);
	desc->nbProcessorsAvailable = value;
}

int Description_getRepetition (SDescription *desc)
{
	assert (desc);
	int res = desc->repetition;
	return res;
}

void Description_setRepetition (SDescription *desc, int value)
{
	assert (desc);
	desc->repetition = value;
}

int Description_getMetaRepetition (SDescription *desc)
{
	assert (desc);
	int res = desc->metaRepetition;
	return res;
}

void Description_setMetaRepetition (SDescription *desc, int value)
{
	assert (desc);
	desc->metaRepetition = value;
}

int Description_getPageSize (SDescription *desc)
{
	assert (desc);
	int res = desc->pageSize;
	return res;
}

void Description_setPageSize (SDescription *desc, int value)
{
	assert (desc);
	desc->pageSize = value;
}

char *Description_getBaseName (SDescription *desc)
{
	assert (desc);
	return desc->baseName;
}

void Description_setBaseName (SDescription *desc, char *value)
{
	assert (desc);

	free (desc->baseName), desc->baseName = NULL;
	if (value != NULL)
	{
		desc->baseName = strDuplicate (value, STRBUF_MAXLEN);
	}
}

void Description_setOmpPath (SDescription *desc, const char *value)
{
	assert (desc);

	free (desc->ompPath), desc->ompPath = NULL;
	if (value != NULL)
	{
		desc->ompPath = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getOmpPath (SDescription *desc)
{
	assert (desc);

	return desc->ompPath;
}

void Description_setOutputPath (SDescription *desc, const char *value)
{
	assert (desc);

	free (desc->outputPath), desc->outputPath = NULL;
	if (value != NULL)
	{
		desc->outputPath = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getOutputPath (SDescription *desc)
{
	assert (desc);
	return desc->outputPath;
}

int Description_getNbPagesForCollision (SDescription *desc)
{
	assert (desc);
	int res = desc->nbPagesForCollision;
	return res;
}

void Description_setNbPagesForCollision (SDescription *desc, int value)
{
	assert (desc);
	desc->nbPagesForCollision = value;
}

int* Description_getVector (SDescription* desc, int idx)
{
	assert (desc);

	if (idx < 0 || idx >= Description_getNbVectors (desc))
		return NULL;
	return &(desc->vectorDescriptor[idx][0]);
}

int Description_getNbVectors (SDescription *desc)
{
	assert (desc);
	return desc->nbVector;
}

void Description_setNbVectors (SDescription *desc, int value)
{
	int nbVectors = Description_getNbVectors (desc);
	int i;
	assert (desc);
	
	if (value < 1)
	{
		desc->nbVector = value;
		return;
	}

	// Now generate the vectorDescriptor
	for (i = 0; i < nbVectors; i++)
	{
		free (desc->vectorDescriptor[i]), desc->vectorDescriptor[i] = NULL;
		free (desc->temp_values.alignments), desc->temp_values.alignments = NULL;
	}
	free (desc->vectorDescriptor), desc->vectorDescriptor = NULL;
	free (desc->temp_values.alignments), desc->temp_values.alignments = NULL;
	free (desc->vectorSizes), desc->vectorSizes = NULL;
	
	/* Vector descriptor array */
	desc->nbVector = value;
	desc->vectorDescriptor = malloc (sizeof (* (desc->vectorDescriptor)) * value);
	assert (desc->vectorDescriptor);
	memset (desc->vectorDescriptor, 0, sizeof ( (* (desc->vectorDescriptor))) * value);
	
	/* Alignments for resume system */
	desc->temp_values.alignments = malloc ( value * sizeof (*desc->temp_values.alignments));
	assert (desc->temp_values.alignments != NULL);
	memset (desc->temp_values.alignments, 0, value * sizeof((*desc->temp_values.alignments)));
	
	/* Customed vectors sizes array */
	desc->vectorSizes = malloc ( value * sizeof (*desc->vectorSizes));
	assert (desc->vectorSizes != NULL);
	memset (desc->vectorSizes, 0, value * sizeof (*desc->vectorSizes));
	
	if (!desc->isNbSizesDefined)
	{
		Description_setVectorSizes (desc, Description_getStartVectorSize (desc));
	}

	//Now for each element
	for (i = 0; i < value; i++)
	{
		//We need 3 elements : start, stop, step
		desc->vectorDescriptor[i] = malloc (sizeof (* (desc->vectorDescriptor[0])) * 3);
		assert (desc->vectorDescriptor[i]);
		memset (desc->vectorDescriptor[i], 0, sizeof ( (* (desc->vectorDescriptor[0]))) * 3);
	}
}

void Description_setKernelFileName (SDescription *desc, char *name)
{
	assert (desc);
	
	free (desc->kernelFileName), desc->kernelFileName = NULL;
	if (name != NULL)
	{
		desc->kernelFileName = strDuplicate (name, STRBUF_MAXLEN);
	}
}

char *Description_getKernelFileName (SDescription *desc)
{
	assert (desc);
	if (desc->nbKernels == 0)
	{
		return desc->kernelFileName;
	}
	assert (desc->kernelCurrentId < desc->nbKernels);
	return desc->kernelFileNames[desc->kernelCurrentId];
}

void Description_setExecFileName (SDescription *desc, char *name)
{
	assert (desc);
	free (desc->execFileName), desc->execFileName = NULL;
	if (name != NULL)
	{
		desc->execFileName = strDuplicate (name, STRBUF_MAXLEN);
	}
}

char *Description_getExecFileName (SDescription *desc)
{
	assert (desc);
	return desc->execFileName;
}

void Description_setExecArgc (SDescription *desc, int argc)
{
	assert (desc);
	desc->execArgc = argc;
}

int Description_getExecArgc (SDescription *desc)
{
	assert (desc);
	return desc->execArgc;
}

void Description_setExecArgv (SDescription *desc, char **argv)
{
	assert (desc);
	desc->execArgv = argv;
}

char **Description_getExecArgv (SDescription *desc)
{
	assert (desc);
	return desc->execArgv;
}

void Description_setDynamicFunctionName (SDescription *desc, char *name)
{
	assert (desc);
	free (desc->dynlibFunc), desc->dynlibFunc = NULL;
	if (name != NULL)
	{
		desc->dynlibFunc = strDuplicate (name, STRBUF_MAXLEN);
	}
}

void Description_setDynamicLibraryName (SDescription *desc, char *name, int shouldDelete)
{
	assert (desc);

	free (desc->dynlibName), desc->dynlibName = NULL;
	if (name != NULL)
	{
		desc->dynlibName = strDuplicate (name, STRBUF_MAXLEN);
		desc->dynlibDelete = shouldDelete;
	}
}

char *Description_getDynamicLibraryName (SDescription *desc)
{
	assert (desc);
	char *tmp = desc->dynlibName;
	char *kernelName = Description_getKernelFileName (desc);
	const Source_type sc = Description_getSourceType (desc);

	//First check if the library name has been set
	if (tmp != NULL)
	{
		return tmp;
	}

	//Check if we don't have a kernel defined
	if (kernelName == NULL)
	{
		return NULL;
	}

	//If not a so, we have work to do
	if (sc == LIBRARY_FILE)
	{
		//Actually have nothing to do
		Description_setDynamicLibraryName (desc, kernelName, 0);
		tmp = desc->dynlibName;
	}
	else
	{
		// If kernelName type is unknown or exec type
		return NULL;
	}

	assert (tmp != NULL);
	return tmp;
}

char *Description_getDynamicFunctionName (SDescription *desc)
{
	assert (desc);
	return desc->dynlibFunc;
}

void Description_setCPUDest (SDescription *desc, int value)
{
	assert (desc);
	desc->CPUDest = value;
}

int Description_getCPUDest (SDescription *desc)
{
	assert (desc);
	int res = desc->CPUDest;
	return res;
}

void Description_setTubeSF (SDescription *desc, int* value)
{
	assert(desc);
	desc->tubeSF = value;
}

int *Description_getTubeSF (SDescription *desc)
{
	assert (desc);
	int *res = desc->tubeSF;
	return res;
}

void Description_setTubeFS (SDescription *desc, int* value)
{
	assert(desc);
	desc->tubeFS = value;
}

int *Description_getTubeFS (SDescription *desc)
{
	assert (desc);
	int *res = desc->tubeFS;
	return res;
}

void Description_setKernelFunction (SDescription *desc, void* value)
{
	assert (desc);
	desc->kernelFunction = value;
}

void *Description_getKernelFunction (SDescription *desc)
{
	assert (desc);
	return desc->kernelFunction;
}

void Description_setLogVerbosity (SDescription *desc, int value)
{
	assert (desc);
	desc->logVerbosity = value;
}

int Description_getLogVerbosity (SDescription *desc)
{
	assert (desc);
	int res = desc->logVerbosity;
	return res;
}

void Description_setLogOutput (SDescription *desc, FILE *value)
{
	assert (desc);
	desc->logOutput = value;
}

FILE* Description_getLogOutput (SDescription *desc)
{
	assert (desc);
	FILE* res = desc->logOutput;
	return res;
}

void Description_setEvaluationCloseFunction (SDescription *desc, evaluationLogisticFctClose value, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	desc->evaluationClose[idX] = value;
}

evaluationLogisticFctClose Description_getEvaluationCloseFunction (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->evaluationClose[idX];
}

void Description_setEvaluationInitFunction (SDescription *desc, evaluationLogisticFctInit value, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	desc->evaluationInit[idX] = value;
}

evaluationLogisticFctInit Description_getEvaluationInitFunction (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->evaluationInit[idX];
}

void Description_setEvaluationStopFunction (SDescription *desc, evaluationFct value, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	desc->evaluationStop[idX] = value;
}

evaluationFct Description_getEvaluationStopFunction (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->evaluationStop[idX];
}

void Description_setEvaluationStartFunction (SDescription *desc, evaluationFct value, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	desc->evaluationStart[idX] = value;
}

evaluationFct Description_getEvaluationStartFunction (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->evaluationStart[idX];
}

void Description_setEvaluationLibraryName (SDescription *desc, char* value, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	free (desc->evaluationLibraryName[idX]), desc->evaluationLibraryName[idX] = NULL;
	if (value != NULL)
	{
		desc->evaluationLibraryName[idX] = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getEvaluationLibraryName (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	return desc->evaluationLibraryName[idX];
}

void Description_setEvaluationLibraryOverheadFlag (SDescription *desc, evaluationFlag *ptr, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);

	desc->evaluationOverheadFlags[idX] = ptr;
}

evaluationFlag Description_getEvaluationLibraryOverheadFlag (SDescription *desc, unsigned idX)
{
	assert (desc);
	assert (idX < desc->nbEvalLibs);
	
	// by default, overhead matters
	if (desc->evaluationOverheadFlags[idX] == NULL)
	{
		return 1;
	}
	
	return *desc->evaluationOverheadFlags[idX];
}

void Description_setSourceType (SDescription *desc, Source_type sc)
{
	assert (desc);
	desc->sourceType = sc;
}

Source_type Description_getSourceType (SDescription *desc)
{
	assert (desc);
	if (desc->sourceType != UNKNOWN_FILE)
	{
		return desc->sourceType;
	}
	
	char *kernelName = Description_getKernelFileName (desc);
	char *execName = Description_getExecFileName (desc);
	Source_type sc;
	
	if(kernelName != NULL && strstr (kernelName, ".so") != NULL)
	{
		sc = LIBRARY_FILE;
	}
	else if (kernelName != NULL && strstr (kernelName, ".c") != NULL)
	{
		sc = SOURCE_FILE;
	}
	else if (kernelName != NULL && strstr (kernelName, ".s") != NULL)
	{
		sc = ASSEMBLY_FILE;
	}
	else if (kernelName != NULL && strstr (kernelName, ".o") != NULL)
	{
		sc = OBJECT_FILE;
	}
	else if (execName != NULL)
	{
		sc = EXECUTABLE_FILE;
	}
	else
	{
		sc = UNKNOWN_FILE;
	}
	
	desc->sourceType = sc;
	
	return sc;
}

int Description_pinCPU (SDescription *desc)
{
	//Pin the thread on the CPU 0
	cpu_set_t cpuset;
		printf("test");
	pid_t myself = getpid ();

	int cpuDest = Description_getCPUDest (desc);

		/* Set affinity mask to include CPU 0 */
		CPU_ZERO(&cpuset);
		CPU_SET(cpuDest, &cpuset);

		int s = sched_setaffinity (myself, sizeof(cpu_set_t), &cpuset);

		if (s != 0)
		{
			unsigned nbCoresDetected = getNbProcessorsAvailable ();
			Log_output (-1, "Error: Cannot schedule the program on the requested core %d : ", cpuDest);
			perror ("");
			Log_output (-1, "Info: Microlauncher detected %d cores on your machine (CPU [0 to %d] assignable).\n", nbCoresDetected, nbCoresDetected-1);
			return -1;
		}

#if 0
		/* Check the actual affinity mask assigned to the myself */
		int cnt = 0;
		s = sched_getcpu ();
		while (s != cpuDest)
		{
			Log_output (1, "Waiting for correct pinning\n");
			sleep (1);
			s = sched_getcpu ();
			cnt++;

			if (s == -1)
			{
				perror ("Error: Cannot get CPU using sched_cpu ()");
				return -1;
			}

			if (cnt > 10)
			{
				Log_output (1, "Warning: Cannot send program to CPU %d\n", cpuDest);
				break;
			}
		}
#endif
        return 0;
}

void Description_setupLog (SDescription *desc)
{
	assert (desc != NULL);
	Log_setVerbosity (Description_getLogVerbosity (desc));

	FILE *output = Description_getLogOutput (desc);

	//If not defined, send to stderr
	if (output != NULL)
	{
		Log_setOutput (output);
	}
	else
	{
		Log_setOutput (stderr);
	}
}


int Description_AssertValues (SDescription *desc)
{
	struct stat st;
	char use_microlaunch_h[] = "Please check \"./microlaunch -h\" for more details about Microlauncher use.\n";
	int sc = Description_getSourceType (desc);
	assert (desc);
	
	/* Si mode exec, alors pas de mode kernel */
	if ( desc->kernelFileName != NULL && desc->execFileName != NULL)
	{
		Log_output (-1, "Error: You can't have both --kernelname and --execname arguments defined.\n");
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	/* If no launching mode is defined */
	if (desc->kernelFileName == NULL && desc->execFileName == NULL)
	{
		Log_output (-1, "Error: You must at least give Microlaunch a --kernelname or a --execname argument.\n");
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	/* If mode kernel, then the user must define kernel-related options */
	if (desc->kernelFileName != NULL)
	{
		if (desc->startVectorSize == DEFAULT_ARRAY_SIZE && desc->endVectorSize != DEFAULT_ARRAY_SIZE)
		{
			Log_output (-1, "Error: You must define a --startvector argument if you want to use an --endvector argument.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->startVectorSize <= 0)
		{
			Log_output (-1, "Error: Startvector argument value must be greater than 0 (given value : %d).\n", desc->startVectorSize);
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->endVectorSize <= 0)
		{
			Log_output (-1, "Error: Endvector argument value must be greater than 0 (given value : %d).\n", desc->endVectorSize);
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->dummySize <= 0)
		{
			Log_output (-1, "Error: Sizedummy argument value must be greater than 0 (given value : %d).\n", desc->dummySize);
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->maxStride <= 0)
		{
			Log_output (-1, "Error: Maxstride argument value must be greater than 0 (given value : %d).\n", desc->maxStride);
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->startVectorSize > desc->endVectorSize)
		{
			Log_output (-1, "Error: startvector can't be upper than endvector\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->vectorSizeStep <= 0)
		{
			Log_output (-1, "Error: stepvector must have a positive value.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (Description_isIterationCountEnabled(desc) && desc->iterationCount <= 0)
		{
			Log_output (-1, "Error: IterationCount argument value must be greater than 0 (given value : %d).\n", desc->iterationCount);
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		/* In case we only have one single vector size */
		if (!Description_isNbSizeDefined (desc))
		{
			if (desc->iterationCount > desc->startVectorSize)
			{
				Log_output (-1, "Error: iteration count value cannot be upper than startvector value\n");
				Log_output (-1, use_microlaunch_h);
				return -1;
			}
		}
		/* In case we have multiple vector sizes (--nbsizes option) */
		else
		{
			if ((unsigned) desc->iterationCount > minUL (desc->vectorSizes, Description_getNbVectors (desc)))
			{
				Log_output (-1, "Error: Iteration count value cannot be upper than any of the vector sizes\n");
				Log_output (-1, use_microlaunch_h);
				return -1;
			}
		}
		
		if (desc->nbVector < 0)
		{
			Log_output (-1, "Error: Nbvector has to be greater than zero\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->nbVector > 8)
		{
			Log_output (-1, "Error: Microlauncher does not support a number of vectors upper than 8.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if ((desc->startVectorSize != DEFAULT_ARRAY_SIZE || desc->endVectorSize != DEFAULT_ARRAY_SIZE) && desc->isNbSizesDefined)
		{
			Log_output (-1, "startvector = %d, endvector = %d, DEFAULT_ARRAY_SIZE = %d\n", desc->startVectorSize, desc->endVectorSize, DEFAULT_ARRAY_SIZE);
			Log_output (-1, "Error: Cannot use --startvector/--endvector arguments and --nbsizes argument at the same time.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (strcmp (desc->kernelFileName, "") == 0)
		{
			Log_output (-1, "Error: kernelname argument cannot be empty.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		/* 01/08/2011 : Should not occur anymore as we define a "entryPoint" kernelfunction by default */
		if (desc->dynlibFunc == NULL || strcmp (desc->dynlibFunc, "") == 0)
		{
			Log_output (-1, "Error: Microlauncher misses a \"--kernelfunction\" argument.\n");
			
			/* if the source file is a dyn library, then try to get info about functions inside the document */
			if (sc == LIBRARY_FILE)
			{
				char syscall[STRBUF_MAXLEN];
				int res;
				Log_output (-1, "Here is a list of detected functions inside your file :\n");
				snprintf (syscall, STRBUF_MAXLEN, "nm -D %s | grep \"^[0-9]* T\" | cut -f 3 -d ' '|grep -v \"^_\"|sed -e 's/^./\t&/g'", desc->kernelFileName);
				res = system (syscall);
				assert (res == EXIT_SUCCESS);
			}
			
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (strcmp (desc->evaluationLibraryName[0], "") == 0)
		{
			Log_output (-1, "Error: evallib argument cannot be empty.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
		
		if (desc->vectorElementSize < 0)
		{
			Log_output (-1, "Error: --data-size argument cannot have a negative value.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
	}
	
	/* If exec mode, then the user must define exec mode arguments */
	if (desc->execFileName != NULL)
	{
		if (strcmp (desc->execFileName, "") == 0)
		{
			Log_output (-1, "Error: execname argument cannot be empty.\n");
			Log_output (-1, use_microlaunch_h);
            return -1;
		}
		
		if (desc->outputFileStream != NULL && desc->suppressOutput == 1)
		{
			Log_output (-1, "Error: Cannot have --log-output and --suppress-output defined at the same time.\n");
			Log_output (-1, use_microlaunch_h);
			return -1;
		}
	}
	
	/* If both */
	/* We're checking outputPath existence... */
	if (stat (desc->outputPath, &st) != 0)
	{
		Log_output (-1, "Warning: Output directory \"%s\" does not exist, Microlaunch is going to create it.\n", desc->outputPath);
		if (mkdir (desc->outputPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
		{
			Log_output (-1, "Error: Cannot create directory \"%s\" : ", desc->outputPath);
			perror ("");
			return -1;
		}
	}
	
	if (desc->executeRepet < 1)
	{
		Log_output (-1, "Error: The --executerepet argument must have a positive value.\n");
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	if (desc->repetition < 1)
	{
		Log_output (-1, "Error: The --repetition argument must have a positive value.\n");
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	if (desc->metaRepetition < 1)
	{
		Log_output (-1, "Error: The --metarepetition argument must have a positive value.\n");
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	if (desc->nbprocess <= 0)
	{
		Log_output (-1, "Error: Nbprocess argument value must be greater than 0 (given value : %d).\n", desc->nbprocess);
		Log_output (-1, use_microlaunch_h);
		return -1;
	}
	
	if ( desc->nbprocess > desc->nbProcessorsAvailable)
	{
		Log_output (-1, "Error: Cannot execute %d processes on this machine (%d processors available detected)\n", desc->nbprocess, desc->nbProcessorsAvailable);
		return -1;
	}
	
	return 0;
}

void Description_setConfigFile (SDescription *desc, char *value)
{
	assert (desc);
	
	if (value != NULL)
	{
		free (desc->configFileName), desc->configFileName = NULL;
		desc->configFileName = strDuplicate (value, STRBUF_MAXLEN);
		if (strstr (value,".xml") == NULL)
		{
			Log_output (-1, "Error: you must specify a *.xml file\n");
			assert (strstr (value, ".xml") != NULL);
		}
	}
}

char *Description_getConfigFile (SDescription *desc)
{
	assert (desc != NULL);
	return desc->configFileName;
}

void Description_setDynAllocLib(SDescription *desc, char* path)
{
	assert (desc);
	free (desc->dynAllocLib), desc->dynAllocLib = NULL;
	if (path != NULL)
	{
		desc->dynAllocLib = strDuplicate (path, STRBUF_MAXLEN);
	}
}

void Description_setMyMallocInit(SDescription *desc, personalized_malloc_init_t ptr)
{
	assert (desc != NULL);
	desc->myMallocInit = ptr;
}

void Description_setMyMalloc(SDescription *desc, personalized_malloc_t ptr)
{
	assert (desc != NULL);
	desc->myMalloc = ptr;
}

void Description_setMyFree(SDescription *desc, personalized_free_t ptr)
{
	assert (desc != NULL);
	desc->myFree = ptr;
}

void Description_setMyMallocDestroy(SDescription *desc, personalized_malloc_destroy_t ptr)
{
	assert (desc != NULL);
	desc->myMallocDestroy = ptr;
}

char* Description_getDynAllocLib(SDescription *desc)
{
	assert (desc != NULL);
	return desc->dynAllocLib;
}

personalized_malloc_init_t Description_getMyMallocInit(SDescription *desc)
{
	assert (desc != NULL);
	return desc->myMallocInit;
}

personalized_malloc_t Description_getMyMalloc(SDescription *desc)
{
	assert (desc != NULL);
	return desc->myMalloc;
}

personalized_free_t Description_getMyFree(SDescription *desc)
{
	assert (desc != NULL);
	return desc->myFree;
}

personalized_malloc_destroy_t Description_getMyMallocDestroy(SDescription *desc)
{
	assert (desc != NULL);
	return desc->myMallocDestroy;
}

int Description_getResuming(SDescription *desc)
{
	assert (desc != NULL);
	return desc->resuming;
}

int Description_getNumber_of_resumes(SDescription *desc)
{
	assert (desc != NULL);
	return desc->number_of_resumes;
}

void Description_setResuming(SDescription *desc, int val)
{
	assert (desc != NULL);
	desc->resuming = val;
}

void Description_setNumber_of_resumes(SDescription *desc, int val)
{
	assert (desc != NULL);
	desc->number_of_resumes = val;
}

int Description_getResumeId(SDescription *desc)
{
	assert (desc != NULL);
	return desc->resumeId;
}

void Description_setResumeId(SDescription *desc, int val)
{
	assert (desc != NULL);
	desc->resumeId = val;
}

void Description_setSuppressOutput (SDescription *desc, int val)
{
	assert (desc != NULL);
	desc->suppressOutput = val;
}

int Description_getSuppressOutput (SDescription *desc)
{
	assert (desc != NULL);
	return desc->suppressOutput;
}

void Description_setOutputFileStream (SDescription *desc, char *value)
{
	assert (desc);

	free (desc->outputFileStream), desc->outputFileStream = NULL;
	if (value != NULL)
	{
		desc->outputFileStream = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getOutputFileStream (SDescription *desc)
{
	assert (desc != NULL);
	return desc->outputFileStream;
}

void Description_setKernelInitFunctionName (SDescription *desc, char *value)
{
	assert (desc != NULL);
	
	free (desc->kernelInitFunctionName), desc->kernelInitFunctionName = NULL;
	if (value != NULL)
	{
		desc->kernelInitFunctionName = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getKernelInitFunctionName (SDescription *desc)
{
	assert (desc != NULL);
	return desc->kernelInitFunctionName;
}

void Description_setKernelInitFunction (SDescription *desc, void *func)
{
	assert (desc != NULL);
	desc->kernelInitFunction = func;
}

void *Description_getKernelInitFunction (SDescription *desc)
{
	assert (desc != NULL);
	return desc->kernelInitFunction;
}

void Description_setVectorElementSize (SDescription *desc, int value)
{
	assert (desc != NULL);
	desc->vectorElementSize = value;
}

int Description_getVectorElementSize (SDescription *desc)
{
	assert (desc != NULL);
	return desc->vectorElementSize;
}

void Description_setVectorSizeAt (SDescription *desc, int value, int idX)
{
	assert (desc != NULL);
	assert (idX < Description_getNbVectors (desc));
	assert (desc->vectorSizes != NULL);
	desc->vectorSizes[idX] = value;
}

void Description_setPinningAt (SDescription *desc, int value, int idX)
{
	assert (desc != NULL);
	assert (idX < Description_getNbProcess (desc));
	assert (desc->pinning != NULL);
	desc->pinning[idX] = value;
}

void Description_setVectorSizes (SDescription *desc, int value)
{
	int i, nbVectors;
	assert (desc != NULL);
	
	if (desc->vectorSizes == NULL)
	{
		return;
	}
	
	nbVectors = Description_getNbVectors (desc);
	
	for (i = 0; i < nbVectors; i++)
	{
		desc->vectorSizes[i] = value;
	}
}

int Description_getVectorSizeAt (SDescription *desc, int idX)
{
	// Paranoid but I think these assertions are important for this one
	assert (desc != NULL);
	assert (idX < Description_getNbVectors (desc));
	assert (desc->vectorSizes != NULL);
	
	return desc->vectorSizes[idX];
}

int Description_getPinningAt (SDescription *desc, int idX)
{
	// Paranoid but I think these assertions are important for this one
	assert (desc != NULL);
	assert (idX < Description_getNbProcess (desc));
	assert (desc->pinning != NULL);
	
	return desc->pinning[idX];
}

int Description_isNbSizeDefined (SDescription *desc)
{
	assert (desc != NULL);
	return desc->isNbSizesDefined;
}

void Description_nbSizesEnable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isNbSizesDefined = 1;
}

void Description_nbSizesDisable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isNbSizesDefined = 0;
}

void Description_setEvaluationData (SDescription *desc, void *data, unsigned idX)
{
	assert (desc != NULL);
	assert (idX < desc->nbEvalLibs);
	desc->evalData[idX] = data;
}

void *Description_getEvaluationData (SDescription *desc, unsigned idX)
{
	assert (desc != NULL);
	assert (idX < desc->nbEvalLibs);
	return desc->evalData[idX];
}

void Description_pinThreadEnable (SDescription *desc)
{
	assert (desc != NULL);
	desc->threadPin = 1;
}

void Description_pinThreadDisable (SDescription *desc)
{
	assert (desc != NULL);
	desc->threadPin = 0;
}

int Description_isThreadPinningEnabled (SDescription *desc)
{
	assert (desc != NULL);
	return desc->threadPin;
}

void Description_allProcessOutputEnable (SDescription *desc)
{
	assert (desc != NULL);
	desc->allProcessOutput = 1;
}

void Description_allProcessOutputDisable (SDescription *desc)
{
	assert (desc != NULL);
	desc->allProcessOutput = 0;
}

int Description_isAllProcessOutputEnabled (SDescription *desc)
{
	assert (desc != NULL);
	return desc->allProcessOutput;
}

void Description_printingProcessEnable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isPrintingProcess = 1;
}

void Description_printingProcessDisable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isPrintingProcess = 0;
}

int Description_isPrintingProcess (SDescription *desc)
{
	assert (desc != NULL);
	return desc->isPrintingProcess;
}

void Description_processEvalHandlerEnable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isEvalHandlerProcess = 1;
}

void Description_processEvalHandlerDisable (SDescription *desc)
{
	assert (desc != NULL);
	desc->isEvalHandlerProcess = 0;
}

int Description_isProcessEvalHandler (SDescription *desc)
{
	assert (desc != NULL);
	return desc->isEvalHandlerProcess;
}

void Description_setProcessId (SDescription *desc, int value)
{
	assert (desc != NULL);
	desc->processId = value;
}

int Description_getProcessId (SDescription *desc)
{
	assert (desc != NULL);
	return desc->processId;
}

void Description_enableSummary (SDescription *desc)
{
	assert (desc != NULL);
	desc->summary = 1;
}

void Description_disableSummary (SDescription *desc)
{
	assert (desc != NULL);
	desc->summary = 0;
}

int Description_isSummaryEnabled (SDescription *desc)
{
	assert (desc != NULL);
	return desc->summary;
}

void Description_defineCpuPin (SDescription *desc)
{
	assert (desc != NULL);
	desc->cpupinDefined = 1;
}

void Description_undefineCpuPin (SDescription *desc)
{
	assert (desc != NULL);
	desc->cpupinDefined = 0;
}

int Description_isCpuPinDefined (SDescription *desc)
{
	assert (desc != NULL);
	return desc->cpupinDefined;
}

void Description_setVerificationLibraryName (SDescription *desc, char *value)
{
	assert (desc != NULL);
	
	free (desc->verificationLibraryName), desc->verificationLibraryName = NULL;
	if (value != NULL)
	{
		desc->verificationLibraryName = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getVerificationLibraryName (SDescription *desc)
{
	assert (desc != NULL);
	return desc->verificationLibraryName;
}

void Description_setVerificationContextData (SDescription *desc, void *data)
{
	assert (desc != NULL);
	desc->verificationContextData = data;
}

void *Description_getVerificationContextData (SDescription *desc)
{
	assert (desc != NULL);
	return desc->verificationContextData;
}

void Description_setVerificationInitFct (SDescription *desc, verificationFctInit fct)
{
	assert (desc != NULL);
	desc->verificationInit = fct;
}

verificationFctInit Description_getVerificationInitFct (SDescription *desc)
{
	assert (desc != NULL);
	return desc->verificationInit;
}

void Description_setVerificationDisplayFct (SDescription *desc, verificationFctDisplay fct)
{
	assert (desc != NULL);
	desc->verificationDisplay = fct;
}

verificationFctDisplay Description_getVerificationDisplayFct (SDescription *desc)
{
	assert (desc != NULL);
	return desc->verificationDisplay;
}

void Description_setVerificationDestroyFct (SDescription *desc, verificationFctDestroy fct)
{
	assert (desc != NULL);
	desc->verificationDestroy = fct;
}

verificationFctDestroy Description_getVerificationDestroyFct (SDescription *desc)
{
	assert (desc != NULL);
	return desc->verificationDestroy;
}

void Description_setKernelFileNamesTabSize (SDescription *desc, unsigned value)
{
	assert (desc != NULL);
	assert (desc->nbKernels == 0); /* Mustn't redefine this value in the program */
	desc->nbKernels = value;
	
	free (desc->kernelFileNames), desc->kernelFileNames = NULL;
	desc->kernelFileNames = malloc ( value * sizeof (*desc->kernelFileNames));
	assert (desc->kernelFileNames != NULL);
	memset (desc->kernelFileNames, 0, value * sizeof (*desc->kernelFileNames));
}

unsigned Description_getKernelFileNamesTabSize (SDescription *desc)
{
	assert (desc != NULL);
	return desc->nbKernels;
}

void Description_setKernelFileNameAt (SDescription *desc, char *value, unsigned idX)
{
	assert (desc != NULL && value != NULL);
	assert (desc->kernelCurrentId < desc->nbKernels);
	assert (desc->kernelFileNames != NULL);
	
	free (desc->kernelFileNames[idX]), desc->kernelFileNames[idX] = NULL;
	if (value != NULL)
	{
		desc->kernelFileNames[idX] = strDuplicate (value, STRBUF_MAXLEN);
	}
}

void Description_setKernelCurrentFileId (SDescription *desc, unsigned value)
{
	assert (desc != NULL);
	desc->kernelCurrentId = value;
}

unsigned Description_getKernelCurrentFileId (SDescription *desc)
{
	assert (desc != NULL);
	return desc->kernelCurrentId;
}

void Description_setNbEvaluationLibrairies (SDescription *desc, unsigned value)
{
	assert (desc != NULL);
	assert (desc->nbEvalLibs == 0); /* Mustn't be redefined */
	
	desc->nbEvalLibs = value;
	
	free (desc->evaluationLibraryName), desc->evaluationLibraryName = NULL;
	desc->evaluationLibraryName = malloc ( value * sizeof (*desc->evaluationLibraryName));
	assert (desc->evaluationLibraryName != NULL);
	memset (desc->evaluationLibraryName, 0, value * sizeof (*desc->evaluationLibraryName));
	
	free (desc->evaluationInit), desc->evaluationInit = NULL;
	desc->evaluationInit = malloc ( value * sizeof (*desc->evaluationInit));
	assert (desc->evaluationInit != NULL);
	memset (desc->evaluationInit, 0, value * sizeof (*desc->evaluationInit));
	
	free (desc->evaluationClose), desc->evaluationClose = NULL;
	desc->evaluationClose = malloc ( value * sizeof (*desc->evaluationClose));
	assert (desc->evaluationClose != NULL);
	memset (desc->evaluationClose, 0, value * sizeof (*desc->evaluationClose));
	
	free (desc->evaluationStart), desc->evaluationStart = NULL;
	desc->evaluationStart = malloc ( value * sizeof (*desc->evaluationStart));
	assert (desc->evaluationStart != NULL);
	memset (desc->evaluationStart, 0, value * sizeof (*desc->evaluationStart));
	
	free (desc->evaluationStop), desc->evaluationStop = NULL;
	desc->evaluationStop = malloc ( value * sizeof (*desc->evaluationStop));
	assert (desc->evaluationStop != NULL);
	memset (desc->evaluationStop, 0, value * sizeof (*desc->evaluationStop));
	
	free (desc->evaluationOverheadFlags), desc->evaluationOverheadFlags = NULL;
	desc->evaluationOverheadFlags = malloc ( value * sizeof (*desc->evaluationOverheadFlags));
	assert (desc->evaluationOverheadFlags != NULL);
	memset (desc->evaluationOverheadFlags, 0, value * sizeof (*desc->evaluationOverheadFlags));
	
	free (desc->evalData), desc->evalData = NULL;
	desc->evalData = malloc ( value * sizeof (*desc->evalData));
	assert (desc->evalData != NULL);
	memset (desc->evalData, 0, value * sizeof (*desc->evalData));
	
	free (desc->infoDisplayed), desc->infoDisplayed = NULL;
	desc->infoDisplayed = malloc ( value * sizeof (*desc->infoDisplayed));
	assert (desc->infoDisplayed != NULL);
	memset (desc->infoDisplayed, 0, value * sizeof (*desc->infoDisplayed));
}

unsigned Description_getNbEvaluationLibrairies (SDescription *desc)
{
	assert (desc != NULL);
	return desc->nbEvalLibs;
}

void Description_parseEvaluationLibraryInput (SDescription *desc, char *source)
{
	assert (desc != NULL && source != NULL);
	char *start, *ptr;
	unsigned size = 0;
	
	/* If number of evaluation librairies is not set, then do it */
	if (Description_getNbEvaluationLibrairies (desc) == 0)
	{
		start = source;
		while ((ptr = strchr (start, ';')) != NULL)
		{
			size++;
			start = ptr + 1;
		}
		if (strnlen (start, STRBUF_MAXLEN) > 0)
		{
			size++;
		}
	
		Description_setNbEvaluationLibrairies (desc, size);
	}
	
	/* Now, let's parse it */
	start = source;
	size = 0;
	while ((ptr = strchr (start, ';')) != NULL)
	{
		*ptr = '\0';
		Description_setEvaluationLibraryName (desc, start, size);
		start = ptr + 1;
		size++;
	}
	/* Set the last one... */
	if (strnlen (start, STRBUF_MAXLEN) > 0)
	{
		Description_setEvaluationLibraryName (desc, start, size);
	}
}

void Description_parseInfoDisplayedInput (SDescription *desc, char *source)
{
	assert (desc != NULL && source != NULL);
	char *start, *ptr;
	unsigned size = 0;
	
	/* If number of evaluation librairies is not set, then do it */
	if (Description_getNbEvaluationLibrairies (desc) == 0)
	{
		start = source;
		while ((ptr = strchr (start, ';')) != NULL)
		{
			size++;
			start = ptr + 1;
		}
		if (strnlen (start, STRBUF_MAXLEN) > 0)
		{
			size++;
		}
	
		Description_setNbEvaluationLibrairies (desc, size);
	}
	
	/* Now, let's parse it */
	start = source;
	size = 0;
	while ((ptr = strchr (start, ';')) != NULL)
	{
		*ptr = '\0';
		Description_setInfoDisplayed (desc, start, size);
		start = ptr + 1;
		size++;
	}
	/* Set the last one... */
	if (strnlen (start, STRBUF_MAXLEN) > 0)
	{
		Description_setInfoDisplayed (desc, start, size);
	}
}

void Description_enableEvalStack (SDescription *desc)
{
	assert (desc != NULL);
	desc->evalStack = 1;
}

void Description_disableEvalStack (SDescription *desc)
{
	assert (desc != NULL);
	desc->evalStack = 0;
}

int Description_isEvalStackEnabled (SDescription *desc)
{
	assert (desc != NULL);
	return desc->evalStack;
}

void Description_setOutputFileName (SDescription *desc, char *value)
{
	assert (desc != NULL);
	
	free (desc->outputFileName), desc->outputFileName = NULL;
	if (value != NULL)
	{
		desc->outputFileName = strDuplicate (value, STRBUF_MAXLEN);
	}
}

char *Description_getOutputFileName (SDescription *desc)
{
	assert (desc != NULL);
	return desc->outputFileName;
}

void Description_setExecPath (SDescription *desc, char *value)
{
	assert (desc != NULL);
    desc->execPath = strDuplicate (value, STRBUF_MAXLEN);
}

char *Description_getExecPath (SDescription *desc)
{
	assert (desc != NULL);
	return desc->execPath;
}



//Special structure for reformatting
typedef struct sGetSet
{
    char *(*get) (SDescription*);
    void (*set) (SDescription*, char*);
}SGetSet;

void Description_reformatStrings (SDescription *desc, int procId)
{
    char *old, *new;
    unsigned int i;
    unsigned int size;
    

    SGetSet fcts[] = {
                        {Description_getBaseName, Description_setBaseName},
                        {Description_getExecFileName, Description_setExecFileName},
                        {Description_getExecPath, Description_setExecPath},
                    };

    //For each get/setter
    size = sizeof (fcts) / sizeof (fcts[0]);
    for (i = 0; i < size; i++)
    {
        char *(*get) (SDescription*) = fcts[i].get;
        void (*set) (SDescription*, char*) = fcts[i].set;
        //Start with the base name
        old = get (desc);
        new = reformatString (old, procId);
        set (desc, new);

        //Free string
        free (new), new = NULL;
    }

    //Now handle each execution argument
    size = Description_getExecArgc (desc);
    char **argv = Description_getExecArgv (desc);

    for (i = 0; i < size; i++)
    {
        new = reformatString (argv[i], procId);
        free (argv[i]), argv[i] = new;
    }
}

/**
 * @brief Reformat a string
 * @param orig the original string
 * @param procId the process ID
 * @return the new string
 */
char *reformatString (char *orig, int procId_int)
{
    char *res = NULL;
    char *pos = NULL, *tmp = NULL;
    char procID[10];
    int copied = 0;
    int digits;

    if (orig == NULL)
        return NULL;

    //Get that ready
    sprintf (procID, "%d", procId_int);
    digits = strlen (procID);

    //First step, replace %uid with procId
    //Do we have a uid?
    pos = strstr (orig, "%uid");

    while (pos != NULL)
    {
        //Found one, we must replace it and add it to res, where are we up to
        int toCopy = pos - orig;
        tmp = malloc (copied + toCopy + digits + 1);
        assert (tmp);
        *tmp = '\0';

        if (res != NULL)
            strcpy (tmp, res);

        strncat (tmp, orig, toCopy);
        strcat (tmp, procID);

        //Remember it
        free (res), res = NULL;
        res = tmp;

        //Update position
        orig = pos + 4;
        copied = toCopy + digits;

        //Find next position
        pos = strstr (orig, "%uid");
    }

    //Copy end
    tmp = malloc (copied + strlen (orig) + 1);
    assert (tmp);
    *tmp = '\0';

    if (res != NULL)
        strcpy (tmp, res);

    strcat (tmp, orig);

    //Return bash execution of the string
    tmp = reformatUsingBash (tmp);
    return tmp;
}

/**
 * @brief Reformat using a bash execution
 * @param s the original string
 * @return the string obtained by executing it in bash, s is freed
 */
char *reformatUsingBash (char *s)
{
    char buf[4096];
    char *start = "echo \\\"";
    char *end = "\\\"";

    //Create command
    // s if the argument passed to the function
    // The command is 'echo "s"'
    // Since echo is handling some arguments, the following command echo "-n"
    // displays nothing. So, now, the command passed is echo \"s\"
    // This function still dangerous, I suppose (since it can't manage ';')
    char *command = malloc (strlen (s) + strlen (start) + strlen (end) + 1);
    assert (command);
    *command = '\0';
    strcat (command, start);
    strcat (command, s);
    strcat (command, end);

    //Execute echo
    printf("Command : '%s'\n",command);
    FILE *f = popen (command, "r");
    assert (fgets (buf, sizeof (buf), f) != NULL);
    pclose (f);

    char *ptr = strchr (buf, '\n');
    assert (ptr != NULL);
    *ptr = '\0';

    //Free command
    free (command), command = NULL;

    buf[strlen(buf)-1] = '\0'; // Put \0 at the place of the final '"'
    char* finalBuf = buf+1; // Jump other the '"'
    fprintf (stderr, "->>>>>> %s -> %s\n", s, finalBuf);
    //Free s
    free (s), s = NULL;
    
    return strDuplicate (finalBuf, STRBUF_MAXLEN);
}

void Description_setAllPrintOut (SDescription *desc, int v)
{
    assert (desc != NULL);
    desc->allPrintOut = v;
}

int Description_getAllPrintOut (SDescription *desc)
{
    assert (desc != NULL);
    return desc->allPrintOut;
}
