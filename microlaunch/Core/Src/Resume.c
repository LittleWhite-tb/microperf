/*
Copyright (C) 2011 Exascale Research Center

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "Description.h"
#include "Log.h"
#include "Resume.h"

/*  This global variable is really relevant since it allows the induction variables resuming to be set correctly
	only the first time we get into the loop. This variable is then set to 0 (see initCounter, enableResuming et disableResuming) */

static int mIsResuming = 0;

void resumeDisableResuming (void) {
	mIsResuming = 0;
}

void resumeEnableResuming (void) {
	mIsResuming = 1;
}

int resumeInitCounter (unsigned *ptr, unsigned value) {
	if (mIsResuming)
	{
		*ptr = value;
		return 1;
	} else {
		return 0;
	}
}

int resumeInitData (void *dest, void *src, size_t size) {
	if (mIsResuming)
	{
		memcpy (dest, src, size);
		return 1;
	} else {
		return 0;
	}
}

int resumeIsResuming (void) {
	return mIsResuming;
}

int resumeLoad (struct sDescription *desc)
{
	char tmp[512];
	char file_path[512];
	FILE* file;
	int i;
	int tmp_int;

	// Generates the proper file name (depending on the resume id)
	snprintf(file_path, 512, "%s/%s_%d.txt", MLDIR, RESUME_PATH, Description_getResumeId (desc));

	file = fopen(file_path, "r");

	if(file == NULL)
	{
		Log_output (-1, "Error: Cannot open resume data file \"%s\"\n", file_path);
		return -1;
	}

	if(fscanf(file, "startVectorSize= %d\n", &desc->startVectorSize) != 1) return -1;
	if(fscanf(file, "endVectorSize= %d\n", &desc->endVectorSize) != 1) return -1;
	if(fscanf(file, "vectorSizeStep= %d\n", &desc->vectorSizeStep) != 1) return -1;
	if(fscanf(file, "repetition= %d\n", &desc->repetition) != 1) return -1;
	if(fscanf(file, "metaRepetition= %d\n", &desc->metaRepetition) != 1) return -1;
	if(fscanf(file, "executeRepet= %d\n", &desc->executeRepet) != 1) return -1;
	if(fscanf(file, "maxStride= %d\n", &desc->maxStride) != 1) return -1;
	if(fscanf(file, "pageSize= %d\n", &desc->pageSize) != 1) return -1;
	if(fscanf(file, "baseName= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setBaseName (desc, NULL);
	}
	else Description_setBaseName (desc, tmp); // does its own strdup
	if(fscanf(file, "nbPagesForCollision= %d\n", &desc->nbPagesForCollision) != 1) return -1;
	if(fscanf(file, "vectorSpacing= %d\n", &desc->vectorSpacing) != 1) return -1;
	if(fscanf(file, "nbprocess= %d\n", &desc->nbprocess) != 1)
    {
        return -1;
    }
    else
    {
        Description_setNbProcess (desc, desc->nbprocess);
    }

	if(fscanf(file, "nbProcessorsAvailable= %d\n", &desc->nbProcessorsAvailable) != 1) return -1;
	if(fscanf(file, "dummySize= %d\n", &desc->dummySize) != 1) return -1;
	if(fscanf(file, "iterationCount= %d\n", &desc->iterationCount) != 1) return -1;
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: dummy array non aligned

	if(fscanf(file, "dynlibName= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setDynamicLibraryName (desc, NULL, 1);
	}
	else Description_setDynamicLibraryName (desc, strdup(tmp), 1);
	if(fscanf(file, "dynlibFunc= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setDynamicFunctionName (desc, NULL);
	}
	else Description_setDynamicFunctionName (desc, tmp);
	if(fscanf(file, "kernelFileName= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setKernelFileName (desc, NULL);
	}
	else Description_setKernelFileName (desc, tmp);
	if(fscanf(file, "execFileName= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setExecFileName (desc, NULL);
	}
	else Description_setExecFileName (desc, strdup(tmp));
	if(fscanf(file, "execArgc= %d\n", &desc->execArgc) != 1) return -1;
	if (desc->execArgc > 0) desc->execArgv = malloc(desc->execArgc * sizeof(char*));
	else desc->execArgv = NULL;
	assert(desc->execArgc == 0 || desc->execArgv != NULL);
	for(i = 0; i < (signed)desc->execArgc; i++)
	{
		int trash;
		if(fscanf(file, "execArgv %d = %s\n", &trash, tmp) != 1) return -1;
		if(strcmp(tmp, "(null)") == 0)
		{
			desc->execArgv[i] = strdup ("");
		}
		else desc->execArgv[i] = strdup(tmp);
	}
	if(fscanf(file, "dynlibDelete= %d\n", &desc->dynlibDelete) != 1) return -1;
	if(fscanf(file, "experimentNumber= %d\n", &desc->experimentNumber) != 1) return -1;
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: tubeSF
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: tubeFS
	if(fscanf(file, "ompPath= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setOmpPath (desc, NULL);
	}
	else Description_setOmpPath (desc, strdup(tmp));
	if(fscanf(file, "nbVector= %d\n", &tmp_int) != 1) return -1;
	Description_setNbVectors (desc, tmp_int);
	for(i = 0; i < desc->nbVector; i++)
	{
		int trash;
		if(fscanf(file, "vectorDescriptor %d = %d %d %d\n", &trash, &desc->vectorDescriptor[i][0], &desc->vectorDescriptor[i][1], &desc->vectorDescriptor[i][2]) != 4) return -1;
	}
	if(fscanf(file, "logVerbosity= %d\n", &desc->logVerbosity) != 1) return -1;
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: logOutput
	if(fscanf(file, "promptOutputCsv= %d\n", &desc->promptOutputCsv) != 1) return -1;

	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationInit
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationClose
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationStart
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationStop
	if(fscanf(file, "sourceType= %d\n", (int*)&desc->sourceType) != 1) return -1;
	if(fscanf(file, "dynAllocLib= %s\n", tmp) != 1) return -1;
	if(strcmp(tmp, "(null)") == 0)
	{
		Description_setDynAllocLib (desc, NULL);
	}
	else
	{
		Description_setDynAllocLib (desc, tmp);
	}	
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMallocInit
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMalloc
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myFree
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMallocDestroy
	if(fscanf(file, "resuming= %d\n", &desc->resuming) != 1) return -1;
	if(fscanf(file, "number_of_resumes= %d\n", &desc->number_of_resumes) != 1) return -1;
	if(fscanf(file, "resumeId= %d\n", &desc->resumeId) != 1) return -1;


	fclose(file);

	return 0;
}

int resumeLoadCounters (struct sDescription *desc)
{
	unsigned i;
	unsigned nbVectors = Description_getNbVectors (desc);
	char file_path[2000];
	FILE* file;

	// Generates the proper file name (depending on the resume id)
	snprintf(file_path, 2000, "%s/%s_%d.txt", MLDIR, RESUME_PATH_COUNTERS, Description_getResumeId (desc));

	file = fopen(file_path, "r");

	if(file == NULL)
	{
		Log_output(-1, "resumeLoadCounters: Could not open %s.\n", file_path);
		return -1;
	}

	free (desc->temp_values.alignments), desc->temp_values.alignments = NULL;
	desc->temp_values.alignments = malloc(nbVectors * sizeof(int));
	assert(desc->temp_values.alignments != NULL);

	for(i = 0; i < nbVectors; i++)
	{
		int trash;
		if(fscanf(file, "alignment %d = %d\n", &trash, &desc->temp_values.alignments[i]) != 2) return -1;
	}

	if(fscanf(file, "current_vector_size= %d\n", &desc->temp_values.current_vector_size) != 1) return -1;
	if(fscanf(file, "current_meta_repet= %d\n", &desc->temp_values.current_meta_repet) != 1) return -1;
	if(fscanf(file, "current_execute_repets= %d\n", &desc->temp_values.current_execute_repet) != 1) return -1;
	if(fscanf(file, "curruns= %d\n", &desc->temp_values.curruns) != 1) return -1;
	
	fclose(file);

	return 0;

}

int resumeSave(struct sDescription *desc)
{

	int i;
	char file_path[2000];
	FILE* file;

	// Generates the proper file name (depending on the resume id)
	sprintf(file_path, "%s/%s_%d.txt", MLDIR, RESUME_PATH, Description_getResumeId (desc));

	file = fopen(file_path, "w");

	if(file == NULL)
	{
		Log_output(-1, "resumeSave: Could not open %s.\n", file_path);
		return -1;
	}

	fprintf(file, "startVectorSize= %d\n", desc->startVectorSize);
	fprintf(file, "endVectorSize= %d\n", desc->endVectorSize);
	fprintf(file, "vectorSizeStep= %d\n", desc->vectorSizeStep);
	fprintf(file, "repetition= %d\n", desc->repetition);
	fprintf(file, "metaRepetition= %d\n", desc->metaRepetition);
	fprintf(file, "executeRepet= %d\n", desc->executeRepet);
	fprintf(file, "maxStride= %d\n", desc->maxStride);
	fprintf(file, "pageSize= %d\n", desc->pageSize);
	fprintf(file, "baseName= %s\n", desc->baseName);
	fprintf(file, "nbPagesForCollision= %d\n", desc->nbPagesForCollision);
	fprintf(file, "vectorSpacing= %d\n", desc->vectorSpacing);
	fprintf(file, "nbprocess= %d\n", desc->nbprocess);
	fprintf(file, "nbProcessorsAvailable= %d\n", desc->nbProcessorsAvailable);
	fprintf(file, "dummySize= %d\n", desc->dummySize);
	fprintf(file, "iterationCount= %d\n", desc->iterationCount);
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: dummy array aligned
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: dummy array non aligned
	fprintf(file, "dynlibName= %s\n", desc->dynlibName);
	fprintf(file, "dynlibFunc= %s\n", desc->dynlibFunc);
	fprintf(file, "kernelFileName= %s\n", desc->kernelFileName);
	fprintf(file, "execFileName= %s\n", desc->execFileName);
	fprintf(file, "execArgc= %d\n", desc->execArgc);
	for (i = 0; i < (signed)desc->execArgc; i++)
	{
		fprintf(file, "execArgv %d = %s\n", i, desc->execArgv[i]);
	}
	fprintf(file, "dynlibDelete= %d\n", desc->dynlibDelete);
	fprintf(file, "experimentNumber= %d\n", desc->experimentNumber);
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: tubeSF
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: tubeFS
	fprintf(file, "ompPath= %s\n", desc->ompPath);
	fprintf(file, "nbVector= %d\n", desc->nbVector);
	for(i = 0; i < desc->nbVector; i++)
	{
		fprintf(file, "vectorDescriptor %d = %d %d %d\n", i, desc->vectorDescriptor[i][0], desc->vectorDescriptor[i][1], desc->vectorDescriptor[i][2]);
	}
	fprintf(file, "logVerbosity= %d\n", desc->logVerbosity);
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: logOutput
	fprintf(file, "promptOutputCsv= %d\n", desc->promptOutputCsv);
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationInit
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationClose
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationStart
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: evaluationStop
	fprintf(file, "sourceType= %d\n", desc->sourceType);
	fprintf(file, "dynAllocLib= %s\n", desc->dynAllocLib);
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMallocInit
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMalloc
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myFree
	// not relevant to save or load because the values do not need (or need not) to be saved from an execution to the other: myMallocDestroy
	fprintf(file, "resuming= %d\n", 1);
	fprintf(file, "number_of_resumes= %d\n", desc->number_of_resumes);
	fprintf(file, "resumeId= %d\n", desc->resumeId);

	fprintf(file, "#Saving was successful if this line ends with a dot.");
	fclose(file);

	return 0;
}

int resumeSaveCounters(struct sDescription *desc)
{
	int i;
	char file_path[2000];
	FILE* file;

	// Generates the proper file name (depending on the resume id)
	snprintf(file_path, 2000, "%s/%s_%d.txt", MLDIR, RESUME_PATH_COUNTERS, Description_getResumeId (desc));

	file = fopen(file_path, "w");

	if(file == NULL)
	{
		Log_output(-1, "resumeSaveCounters: Could not open \"%s\".\n", file_path);
		return -1;
	}

	for (i = 0; i < desc->nbVector; i++)
	{
		fprintf(file, "alignment %d = %d\n", i, desc->temp_values.alignments[i]);
	}

	fprintf(file, "current_vector_size= %d\n", desc->temp_values.current_vector_size);
	fprintf(file, "current_meta_repet= %d\n", desc->temp_values.current_meta_repet);
	fprintf(file, "current_execute_repets= %d\n", desc->temp_values.current_execute_repet);
	fprintf(file, "curruns= %d\n", desc->temp_values.curruns);

	fprintf(file, "#Saving was successful if this line ends with a dot.");
	fclose(file);

	return 0;
}

void resumeSignalJobDone (SDescription *desc)
{
	FILE* file;
	char file_path[2000];
	assert (desc != NULL);
	
	// Generates the proper file name (depending on the resume id)
	snprintf (file_path, 2000, "%s/%s_%d.txt", MLDIR, RESUME_PATH_JOB_DONE, Description_getResumeId (desc));

	file = fopen (file_path, "w");
	if (file != NULL)
	{
		fclose (file);
	}
	else
	{
		Log_output (-1, "Warning: Cannot open file \"%s\"\n", file_path);
		perror("");
		exit (EXIT_FAILURE);
	}
}
