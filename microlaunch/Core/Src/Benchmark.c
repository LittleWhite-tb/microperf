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
along with this program; if not, write to thWe Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*============= INCLUDES ===================================================*/
	  
#include <assert.h>
#include <dlfcn.h>
#include <sys/io.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "BenchDescriptor.h"
#include "Benchmark.h"
#include "Description.h"
#include "Defines.h" 
#include "Dflush.h"
#include "Log.h"
#include "Progress.h"
#include "Resume.h"
#include "SleepTight.h"
#include "Signal.h"
#include "Toolkit.h"

//Static declaration of functions
static void benchmark_kernel (BenchResult **res, unsigned long *n, void ** Arrays, SDescription *desc, int EnableSync);

static inline BenchResult *BenchResult_create (int meta_repet)
{
	BenchResult *br;
	
	br = malloc (sizeof (*br));
	assert (br != NULL);
	
	br->time = malloc ( meta_repet * sizeof (*br->time));
	br->iterations = malloc ( meta_repet * sizeof (*br->iterations));
	assert (br->iterations != NULL);
	assert (br->time != NULL);
	memset (br->time, 0, meta_repet * sizeof (*br->time));
	memset (br->iterations, 0, meta_repet * sizeof (*br->iterations));
	br->initialTime = 0.;
	br->finalTime = 0.;
	
	return br;
}

static inline void BenchResult_destroy (BenchResult *br)
{
	assert (br != NULL);
	
	free (br->time), br->time = NULL;
	free (br->iterations), br->iterations = NULL;
	free (br), br = NULL;
}

static inline void ClearArrayFloat (uint64_t nbElements, float *pArray)
{
	uint64_t i;
	
	for (i = 0; i < nbElements; i++)
	{
		pArray[i] = 1.0f;
	}
}

static inline void ClearArrayDouble (uint64_t nbElements, double *pArray)
{
	uint64_t i;
	
	for (i = 0; i < nbElements; i++)
	{
		pArray[i] = 1.0f;
	}
}

/**
 * @brief Set all elements to one
 * @param nNB_Elements Number of elements
 * @param pArray Address of the array
 * @param elemSize size of each element of the array (in bytes)
 */
static inline void 
ClearArray (uint64_t nNB_Elements, void* pArray, int elemSize )
{
	switch (elemSize)
	{
		case sizeof (float):
			ClearArrayFloat (nNB_Elements, pArray);
			break;
		case sizeof (double):
			ClearArrayDouble (nNB_Elements, pArray);
			break;
		default:
			memset (pArray, 0, elemSize * nNB_Elements);
			break;
	}
}

static inline double Benchmark_launchEvalFunction (evaluationFct fct, int isProcessEvalHandler, void *evalData) {
	if (fct != NULL && isProcessEvalHandler)
	{
		return fct (evalData);
	}
	return 0.;
}

static inline void Benchmark_launchBenchmark (BenchResult **res, SDescription *desc, unsigned long *vectorSizes, void **arrays,
												kernel_fctptr kernel_run, int enableSync, int storeResult, int idX) {
	int timeIsOk = 0;
	int *pipeSF = Description_getTubeSF (desc);
	int *pipeFS = Description_getTubeFS (desc);
	evaluationFct start;
	evaluationFct stop;
	int nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	int isProcessEvalHandler = Description_isProcessEvalHandler (desc);
	void *func = Description_getKernelFunction (desc);
	unsigned long nbVectors = Description_getNbVectors (desc);
	unsigned elemSize = Description_getVectorElementSize (desc);
	void *evalData;
	int isRoot = Description_getRoot (desc);
	int repetitions;
	int nbRepetitions = Description_getRepetition (desc);
	uint64_t oldIterations = 0;
	uint64_t newIterations = 0;
	int i;
	int isEvalStackEnabled = Description_isEvalStackEnabled (desc);

	// If the file PATH_TO_KILL exists, cancel the program (SleepTight.h)
	escape_from_scary_killers();
		
	if (enableSync == 1)
	{
		barrierS (pipeSF, pipeFS);
	}
		
	pushSignalHandler (SignalHandler_benchmark);
	while (!timeIsOk)
	{
			// Heat up the data and instruction caches
			// Heating also includes timer calls to make sure they've been called once
			for (i = 0; i < nbEvalLibs; i++) /* Eval Start */
			{
				start = Description_getEvaluationStartFunction (desc, i);
				evalData = Description_getEvaluationData (desc, i);
				res[i]->initialTime = Benchmark_launchEvalFunction (start, isProcessEvalHandler, evalData);
			}
			oldIterations = kernel_run (nbVectors, vectorSizes, elemSize, arrays, func);
			
			/* EVAL STOP */
			if (isEvalStackEnabled) /* We're stopping the librairies in the reverse order */
			{
				for (i = nbEvalLibs-1; i >= 0; i--)
				{
					stop = Description_getEvaluationStopFunction (desc, i);
					evalData = Description_getEvaluationData (desc, i);
					res[i]->finalTime = Benchmark_launchEvalFunction (stop, isProcessEvalHandler, evalData);
				}
			}
			else /* Else, we're stopping the librairies sequentially */
			{
				for (i = 0; i < nbEvalLibs; i++)
				{
					stop = Description_getEvaluationStopFunction (desc, i);
					evalData = Description_getEvaluationData (desc, i);
					res[i]->finalTime = Benchmark_launchEvalFunction (stop, isProcessEvalHandler, evalData);
				}
			}
			
			if (isRoot)
			{
					iopl(3);
					asm volatile("cli");
			}

			/* -------- [ REAL BENCH COMPUTATION ] ------------- */
			for (i = 0; i < nbEvalLibs; i++) /* Eval start */
			{
				start = Description_getEvaluationStartFunction (desc, i);
				evalData = Description_getEvaluationData (desc, i);
				res[i]->initialTime = Benchmark_launchEvalFunction (start, isProcessEvalHandler, evalData);
			}
			
			for (repetitions = 0; repetitions < nbRepetitions; repetitions++) /* For each repetition */
			{
				newIterations = kernel_run (nbVectors, vectorSizes, elemSize, arrays, func);
				
				if (oldIterations != newIterations)
				{
						Log_output (25, "Warning: iteration number not the same\n");
				}
				oldIterations = newIterations;
			}
			
			/* EVAL STOP */
			if (isEvalStackEnabled) /* We're stopping the librairies in the reverse order */
			{
				for (i = nbEvalLibs-1; i >= 0; i--)
				{
					stop = Description_getEvaluationStopFunction (desc, i);
					evalData = Description_getEvaluationData (desc, i);
					res[i]->finalTime = Benchmark_launchEvalFunction (stop, isProcessEvalHandler, evalData);
				}
			}
			else
			{
				for (i = 0; i < nbEvalLibs; i++)
				{
				stop = Description_getEvaluationStopFunction (desc, i);
				evalData = Description_getEvaluationData (desc, i);
				res[i]->finalTime = Benchmark_launchEvalFunction (stop, isProcessEvalHandler, evalData);
				}
			}
			
			/* ------------------------------------------------- */

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
	popSignalHandler ();

	if (storeResult)
	{
		/* Store data results for each library evaluation */
		for (i = 0; i < nbEvalLibs; i++)
		{
			res[i]->time[idX] = res[i]->finalTime - res[i]->initialTime;
			res[i]->iterations[idX] = newIterations;
		}
	}
}

/**
 * @brief Benchmark entry point
 * @param n length of the arrays
 * @param Arrays address of the first vector
 * @param desc the SDescription of this execution
 * @param EnableSync Enable/Disable the synchronisation (we are too fast in the overhead computation it leads to unstable behavior)
 * @return the benchmark result (can be NULL)
 */
void
benchmark_kernel (BenchResult **res, unsigned long *vectorSizes, void **arrays, SDescription *desc, int EnableSync)
{
	int coarse_loop;
	int meta_repet = Description_getMetaRepetition (desc);
	unsigned long nbVectors = Description_getNbVectors (desc);
	kernel_fctptr kernelTable[8] = {kernel1, kernel2, kernel3, kernel4, kernel5,kernel6, kernel7, kernel8};
	kernel_fctptr kernel_run;
	verificationFctInit verifyInit = Description_getVerificationInitFct (desc);
	verificationFctDisplay verifyDisplay = Description_getVerificationDisplayFct (desc);
	verificationFctDestroy verifyDestroy = Description_getVerificationDestroyFct (desc);
	void *verifyContextData = NULL;
	double *dummyArray = Description_getDummyArrayAligned (desc);
	unsigned dummySize = Description_getDummySize (desc);
	FILE *fp;
	
	//Associative table for the kernels
	//init of this table
	
	pushSignalHandler (SignalHandler_launchingBenchmark);
	
	/* Selection of the kernel to execute */
	if (Description_isNbSizeDefined (desc))
	{
		kernel_run = kernelMDL;
	}
	else
	{
		if (nbVectors == 0)
		{
			kernel_run = kernelTable[0];
		}
		else
		{
			kernel_run = kernelTable[nbVectors-1];
		}
	}
	
	/* Verifying library support */
	if (verifyInit != NULL && verifyDisplay != NULL && verifyDestroy != NULL)
	{
		/* Open and init verification stuff */
		char buf[STRBUF_MAXLEN];
		
		snprintf (buf, sizeof (buf), "%s/%s", MLDIR, "test_kernel.txt");
		fp = fopen (buf, "w");
		assert (fp != NULL);
		verifyContextData = verifyInit (desc);
		
		/* Launch with verification */
		Benchmark_launchBenchmark (res, desc, vectorSizes, arrays, kernel_run, EnableSync, 0, 0);
		
		/* Display and close verification context */
		verifyDisplay (verifyContextData, fp);
		verifyDestroy (verifyContextData);
		fclose (fp), fp = NULL;
	}
	
	// Meta loop that selects the lowest measure
	for (coarse_loop = 0; coarse_loop < meta_repet ; coarse_loop ++)
	{
		/* Resume system counter saving */
		desc->temp_values.current_meta_repet = coarse_loop;
		
		/* Flush Caches */
		readDummyArray (dummyArray, dummySize);
		
		/* Benchmark launching */
		Benchmark_launchBenchmark (res, desc, vectorSizes, arrays, kernel_run, EnableSync, 1, coarse_loop);
	}
	
	popSignalHandler ();
}

int 
containsCFG (char *s)
{
	char *end = strstr (s, ".cfg");

	//If we found something
	if (end != NULL)
	{
		//If at end of name
		if (end[4] == '\0')
		{
			return 1;
		}
	}

	return 0;
}

/**
 * @brief Returns how many experiments need to be done
 */
int
getExperimentNumber (SDescription *desc)
{
	int nb_experiments = 1;
	int i;
	int nbVect = Description_getNbVectors(desc);
	int *vect;
	int tmp;
	
	/* This loop computes the number of alignments possibilities */
	for (i=0 ; i < nbVect ; i++)
	{
		vect = Description_getVector (desc, i);
		assert (vect != NULL);
		tmp = (vect[VSTOP] - vect[VSTART]) ;
		if (vect[VSTART] < vect[VSTOP] && vect[VSTEP] != 0)
			tmp /= vect[VSTEP];
		nb_experiments *= (tmp+1);
	}
	
	/* Then multiply it by the number of experiments to be launched : ((end-start)/step)+1 */
	nb_experiments *= ((Description_getEndVectorSize (desc) - Description_getStartVectorSize (desc)) / Description_getVectorSizeStep (desc)) +1;
	
	/* If we're using the verification library, then we're doing one more experiment */
	if (Description_getVerificationLibraryName (desc) != NULL)
	{
		nb_experiments++;
	}
	
	return nb_experiments;
}

static inline void *Benchmark_loadBenchmarkFunction (SDescription *desc, int isPrintingProcess) {
	char *dynlibName = Description_getDynamicLibraryName (desc);
	char *dynlibFuncName = Description_getDynamicFunctionName (desc);
	char *kernelInitFunctionName = Description_getKernelInitFunctionName (desc);
	void *benchmarkFct;
	void *benchmarkInitFct;
	void *dl = NULL;
	int sc = Description_getSourceType (desc);
	
	if (dynlibName != NULL && dynlibFuncName != NULL)
	{
		// Dynamic library mode
		dl = dlopen (dynlibName, RTLD_NOW);
		
		if (dl == NULL)
		{
			Log_output (-1, "Error: Library name %s\n", dlerror ());
			return NULL;
		}

		//Clear any error
		dlerror ();
		
		benchmarkFct = dlsym (dl, dynlibFuncName);
		
		if (benchmarkFct == NULL)
		{
			if (isPrintingProcess)
			{
				Log_output (1, "Error: kernel function \"%s\" doesn't exist\n", dynlibFuncName);
				switch (sc)
				{
					case LIBRARY_FILE:
					{
						char syscall[STRBUF_MAXLEN];
						int res;
						unsigned size;
						Log_output (-1, "Here is a list of detected functions inside your file :\n");
						size = snprintf (syscall, sizeof (syscall), "nm -D %s | grep \"^[0-9a-Z]* T\" | cut -f 3 -d ' '|grep -v \"^_\"|sed -e 's/^./\t&/g'", desc->kernelFileName);
						assert (size < sizeof (syscall));
						/* Effective system call */
						res = system (syscall);
						assert (res == EXIT_SUCCESS);
						break;
					}
					case ASSEMBLY_FILE:
					case SOURCE_FILE:
					{
						char syscall[STRBUF_MAXLEN];
						unsigned size;
						int res;
						Log_output (-1, "Here is a list of detected functions inside your file :\n");
						size = snprintf (syscall, sizeof (syscall), "nm -D %s | grep \"^[0-9a-Z]* T\" | cut -f 3 -d ' '|grep -v \"^_\"|sed -e 's/^./\t&/g'", Description_getDynamicLibraryName (desc));
						assert (size < sizeof (syscall));
						/* Effective system call */
						
						fprintf (stderr, "test\n");
						
						res = system (syscall);
						
						fprintf (stderr, "test2\n");
						assert (res == EXIT_SUCCESS);
						
						abort (); /* Here, only this process has to tell the father : OK, die */
						break;
					}
					default:
						break;
				}
			}
			else
			{
				/* In this case, we don't abort and send signal to kill everybody, because if we do so
					the printingProcess wouldn't be able to do it system call */
				exit (EXIT_FAILURE);
			}
		}
		
		if (kernelInitFunctionName != NULL)
		{
			benchmarkInitFct = dlsym (dl, kernelInitFunctionName);
		
			if (benchmarkInitFct == NULL)
			{
				Log_output (-1, "Error: kernel init function \"%s\" doesn't exist\n", kernelInitFunctionName);
				return NULL;
			}
			
			Description_setKernelInitFunction (desc, benchmarkInitFct);
		}

		//Set kernel function
		Description_setKernelFunction (desc, benchmarkFct);
	}
	
	return dl;
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
	
	if (evallibName != NULL)
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
			Log_output (-1, "Warning: Start function \"%s\" is not declared in Evaluation Library.\n", startEval);
		}
		stopFct = dlsym (dl, stopEval);
		if (stopFct == NULL)
		{
			Log_output (-1, "Warning: Stop function \"%s\" is not declared in Evaluation Library.\n", stopEval);
		}
		initFct = dlsym (dl, initEval);
		if (initFct == NULL)
		{
			Log_output (-1, "Warning: Init function \"%s\" is not declared in Evaluation Library.\n", initEval);
		}
		closeFct = dlsym (dl, closeEval);
		if (closeFct == NULL)
		{
			Log_output (-1, "Warning: Close function \"%s\" is not declared in Evaluation Library.\n", closeEval);
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

static inline void *Benchmark_loadVerificationFunctions (SDescription *desc) {
	char *verificationLibName = Description_getVerificationLibraryName (desc);
	char *initVerify = "verificationInit";
	char *displayVerify = "verificationDisplay";
	char *destroyVerify = "verificationClose";
	void *dl = NULL;
	verificationFctInit verifyInitFct;
	verificationFctDisplay verifyDisplayFct;
	verificationFctDestroy verifyDestroyFct;
	
	if (verificationLibName != NULL)
	{
		dl = dlopen (verificationLibName, RTLD_LAZY);
		
		if (dl == NULL)
		{
			Log_output (-1, "Error: Library name %s\n", dlerror ());
			return NULL;
		}

		//Clear any error
		dlerror ();

		verifyInitFct = dlsym (dl, initVerify);
		if (verifyInitFct == NULL)
		{
			Log_output (-1, "Error: Cannot load %s in the verification library %s : %s\n", initVerify, verificationLibName, dlerror ());
			abort ();
		}
		verifyDisplayFct = dlsym (dl, displayVerify);
		if (verifyDisplayFct == NULL)
		{
			Log_output (-1, "Error: Cannot load %s in the verification library %s : %s\n", displayVerify, verificationLibName, dlerror ());
			abort ();
		}
		verifyDestroyFct = dlsym (dl, destroyVerify);
		if (verifyDestroyFct == NULL)
		{
			Log_output (-1, "Error: Cannot load %s in the verification library %s : %s\n", destroyVerify, verificationLibName, dlerror ());
			abort ();
		}

		//Set functions
		Description_setVerificationInitFct (desc, verifyInitFct);
		Description_setVerificationDisplayFct (desc, verifyDisplayFct);
		Description_setVerificationDestroyFct (desc, verifyDestroyFct);
	}
	
	return dl;
}

static inline void *Benchmark_loadAllocationFunctions (SDescription *desc) {
	void *dl;
	char* alloc_path = Description_getDynAllocLib (desc);
	
	//Paranoid: this is supposedly impossible, as a default value is set
	assert(alloc_path != NULL);
	
	dl = dlopen(alloc_path, RTLD_NOW);
	if (dl == NULL)
	{
		Log_output (-1, "Error while loading dynamic allocation library name '%s' : %s\n", alloc_path, dlerror ());
		return NULL;
	}
	else
	{
		personalized_malloc_init_t myMallocInit;
		personalized_malloc_t myMalloc;
		personalized_free_t myFree;
		personalized_malloc_destroy_t myMallocDestroy;

		dlerror ();

		myMallocInit = dlsym (dl, "allocationInit");
		myMalloc = dlsym (dl, "allocationMalloc");
		myFree = dlsym (dl, "allocationFree");
		myMallocDestroy = dlsym (dl, "allocationClose");

		if(	myMallocInit == NULL ||
			myMalloc == NULL ||
			myFree == NULL ||
			myMallocDestroy == NULL	)
		{
			Log_output (-1, "Error while loading dynamic allocation library name : not all functions could be found. (%s)\n", dlerror ());
			abort ();
		}

		Description_setMyMallocInit (desc, myMallocInit);
		Description_setMyMalloc (desc, myMalloc);
		Description_setMyFree (desc, myFree);
		Description_setMyMallocDestroy (desc, myMallocDestroy);
	}
	
	return dl;
}

void Benchmark_makeDummyArray (SDescription *desc) {
	unsigned int m;
	unsigned sizeDummy = Description_getDummySize (desc);
	double *dummy_array = malloc (sizeDummy * sizeof (*dummy_array));

	if (dummy_array == NULL) {
		Log_output (-1, "Error: could not allocate dummy array.\n");
		exit (EXIT_FAILURE);
	}

	/* Real pages allocation is done while writing in memory space */
	for (m = 0 ; m < sizeDummy ; m++)
	{
		dummy_array[m] = 0.0f;
	}

	Description_setDummyArray (desc, dummy_array, dummy_array);
}

/**
 * @brief Close the dynamic librairies opened in the Benchmark wrapper
 * @param dl_bench The dyn lib of the benchmark function
 * @param dl_eval The dyn lib of the evaluation functions
 * @param dl_alloc The dyn lib of the allocation library
 * @return Whether or not the dyn lib were closed normally (-1 on error, 1 else)
 */
static inline int Benchmark_closeLibraries (void *dl_bench, void **dl_eval, unsigned nbEvalLibs, void *dl_alloc, void *dl_verify)
{
	unsigned i;
	
	if (dl_bench != NULL)
	{
		if (dlclose (dl_bench) != 0)
		{
			Log_output (-1, "An error occured while closing dl_bench : %s\n", dlerror ());
			return -1;
		}
		dl_bench = NULL;
	}
	if (dl_eval != NULL)
	{
		for (i = 0; i < nbEvalLibs; i++)
		{
			if (dl_eval[i] != NULL && dlclose (dl_eval[i]) != 0)
			{
				Log_output (-1, "An error occured while closing dl_eval[%d] : %s\n", i, dlerror ());
				return -1;
			}
			dl_eval[i] = NULL;
		}
	}
	if (dl_alloc != NULL)
	{
		if (dlclose (dl_alloc) != 0)
		{
			Log_output (-1, "An error occured while closing dl_alloc : %s\n", dlerror ());
			return -1;
		}
		dl_alloc = NULL;
	}
	if (dl_verify != NULL)
	{
		if (dlclose (dl_verify) != 0)
		{
			Log_output (-1, "An error occured while closing dl_verify : %s\n", dlerror ());
			return -1;
		}
		dl_verify = NULL;
	}
	return 1;
}

inline FILE *Benchmark_createOutputFile (SDescription *desc, int currentExecRepet, int currentVectorSize)
{
	assert (desc != NULL);
	
	FILE *outputCsvFile = NULL;
	int isAllProcessOutputEnabled = Description_isAllProcessOutputEnabled (desc);
	unsigned execRepets = Description_getExecuteRepets (desc);
	char *outputPath = Description_getOutputPath (desc);
	char *basename = Description_getBaseName (desc);
	int isKernelMode = (Description_getExecFileName (desc) == NULL);
	unsigned w_size;
	
	char outputCsvFileName[STRBUF_MAXLEN];
	char coreBuf[STRBUF_MAXLEN];
	
	/* We add a core_%d extension if several processes have to handle eval library */
	if (isAllProcessOutputEnabled)
	{
		w_size = snprintf (coreBuf, sizeof (coreBuf), "_core_%d", Description_getProcessId (desc));
		assert (w_size < sizeof (coreBuf));
	}
	else
	{
		coreBuf[0] = '\0'; /* Just initializes the string with nothing */
	}
	
	if (isKernelMode) /* KERNEL MODE */
	{
		if (execRepets > 1) /* If there are several execute-repetition */
		{
			w_size = snprintf (outputCsvFileName, sizeof (outputCsvFileName), "%s/kernel_execution_%d__%s_%d%s.csv", outputPath, currentExecRepet, basename, currentVectorSize, coreBuf);
		}
		else
		{
			w_size = snprintf (outputCsvFileName, sizeof (outputCsvFileName), "%s/kernel_%s_%d%s.csv", outputPath, basename, currentVectorSize, coreBuf);
		}
	}
	else /* EXEC MODE */
	{
		if (execRepets > 1)
		{
			w_size = snprintf (outputCsvFileName, sizeof (outputCsvFileName), "%s/exec_execution_%d__%s%s.csv", outputPath, currentExecRepet, basename, coreBuf);
		}
		else
		{
			w_size = snprintf (outputCsvFileName, sizeof (outputCsvFileName), "%s/exec_%s%s.csv", outputPath, basename, coreBuf);
		}
	}
	assert (w_size < sizeof (outputCsvFileName));
	Description_setOutputFileName (desc, outputCsvFileName);

	/* Ouverture du fichier csv or not, depend on the presence of --no-output in the command line*/
	outputCsvFile = fopen (outputCsvFileName, "w");
	if (outputCsvFile == NULL)
	{
		Log_output (-1, "Error: Cannot open file %s\n", outputCsvFileName);
		perror ("");
		return NULL;
	}
	
	return outputCsvFile;
}

static inline void initializeSystemState (SDescription *desc, int *systemState)
{
	assert (desc != NULL && systemState != NULL);
	int *vect;
	unsigned i;
	unsigned nbVectors = Description_getNbVectors (desc);
	
	for (i = 0; i < nbVectors; i++)
	{
		vect = Description_getVector (desc, i);
		if (vect[VSTEP] == 0)
		{
			vect[VSTEP] = 1;
		}
		systemState[i] = vect[VSTART];
	}
}

static inline void replaceBaseName (SDescription *desc)
{
	assert (desc != NULL);
	char *slash, *dot;
	
	/* If there is a path to the file */
	if ((slash = strrchr (Description_getKernelFileName (desc), '/')) != NULL)
	{
		dot = strrchr (slash, '.'); /* Search for the last point of the filename (ie its extension) */
		assert (dot != NULL); /* We have to have a file extension */
		*dot = '\0';
		Description_setBaseName (desc, slash+1);
	}
}

static inline void allocateArrays (void **arrays_offset, unsigned nbVectors, unsigned elemSize, int *systemState, SDescription *desc)
{
	int (*benchmarkInitFct) (int, int, void*, size_t) = Description_getKernelInitFunction (desc);
	unsigned i;
	personalized_malloc_t alloc = Description_getMyMalloc (desc);
			
	/* Setting the new arrays offsets */
	for (i = 0; i < nbVectors; i++)
	{
		arrays_offset[i] = alloc (desc->vectorSizes[i] * elemSize, i, systemState[i]);		

		// Putting valid data in our vectors for the overhead run
		if (benchmarkInitFct != NULL)
		{
			benchmarkInitFct (i, desc->vectorSizes[i], arrays_offset[i], elemSize);
		}
		else
		{
			ClearArray (desc->vectorSizes[i],arrays_offset[i], elemSize);
		}
	}
}

static inline void saveOrLoadDataFromResuming (int *curRuns, int *systemState, SDescription *desc, unsigned nbVectors, int isPrintingProcess)
{
	unsigned mloop;
	/* If we just resumed our run, we set the curRuns value to the one which has been saved */
	if (resumeIsResuming ())
	{
		*curRuns = desc->temp_values.curruns;
	}
	
	/* If we just resumed our run, we copy the systemState table to the values we saved */
	if (resumeInitData (systemState, desc->temp_values.alignments, nbVectors * sizeof (*systemState)))
	{
		if (isPrintingProcess)
		{
			Log_output (-1, "Resuming to alignment...\n");
			for (mloop = 0; mloop < nbVectors; mloop++)
			{
				Log_output (-1, "\t- Vector[%d] : %d, %d\n", mloop, systemState[mloop], desc->temp_values.alignments[mloop]);
			}
		}
	}
	else /* else, we save the current alignment set and save the counters */
	{
		memcpy (desc->temp_values.alignments, systemState, nbVectors * sizeof (*systemState));
		if (isPrintingProcess)
		{
			resumeSaveCounters (desc);
		}
	}
	
	/* If we just resumed our run, we set the curRuns value to the one which has been saved */
	if (resumeIsResuming ())
	{
		*curRuns = desc->temp_values.curruns;
	}
}

/*===================== BENCHMARK 2D =======================================*/
int Benchmark_Wrapper (SDescription *desc, int currentExecRepet)
{
	unsigned nCurrentVectorSize = Description_getStartVectorSize (desc);
	unsigned i;
	FILE *outputCsvFile  = NULL;
	unsigned repet = Description_getRepetition (desc);
	unsigned meta_repet = Description_getMetaRepetition (desc);
	BenchResult **overhead;
	BenchResult **res;
	int *systemState;
	void **arrays_offset = NULL;
	unsigned nbVectors = Description_getNbVectors (desc);
	int iterationCountIsEnabled = Description_isIterationCountEnabled (desc);
	int iterationCount = Description_getIterationCount (desc);
	unsigned long *iterationSizes;	/**< @brief the pointer for the good iteration sizes array */
	unsigned long *iterationCountTable = NULL; /**< @brief the sizes for iteration count feature (which is the same for each element) */
	unsigned long *overheadSizes;	/**< @brief the overhead sizes (typically 0 for each element) */
	void *dl_bench, **dl_eval, *dl_alloc, *dl_verify;
	personalized_malloc_init_t alloc_init;
	personalized_free_t alloc_free;
	unsigned end = Description_getEndVectorSize (desc),	 
		step = Description_getVectorSizeStep (desc);
	double *overheadAvg;
	int curRuns = 0;
	int totalRuns = getExperimentNumber (desc);
	int (*benchmarkInitFct) (int, int, void*, size_t);
	int elemSize = Description_getVectorElementSize (desc);
	int maxStride = Description_getMaxStride (desc);
	int timerCloseRes = 0;
	void *timerInitRes = NULL;
	int isPrintingProcess = Description_isPrintingProcess (desc);
	int isProcessEvalHandler = Description_isProcessEvalHandler (desc);
	unsigned nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	evaluationLogisticFctInit init_timer;
	evaluationLogisticFctClose close_timer;
	unsigned evalLoop;
	int isRequestedToMakeFile = Description_getPromptOutputCsv (desc);
	int isNbSizeDefined = Description_isNbSizeDefined (desc);
	int *vect;
	
	/* Vectors allocation */
	systemState = malloc ( nbVectors * sizeof (*systemState));
	arrays_offset = malloc ( nbVectors * sizeof (*arrays_offset));
	overheadSizes = malloc ( nbVectors * sizeof (*overheadSizes));
	dl_eval = malloc ( nbEvalLibs * sizeof (*dl_eval));
	res = malloc ( nbEvalLibs * sizeof (*res));
	overhead = malloc ( nbEvalLibs * sizeof (*res));
	overheadAvg = malloc ( nbEvalLibs * sizeof (*overheadAvg));
	assert (overheadAvg != NULL);
	assert (overhead != NULL);
	assert (res != NULL);
	assert (dl_eval != NULL);
	assert (overheadSizes != NULL);
	assert (arrays_offset != NULL);
	assert (systemState != NULL);
	
	/* Prints out that we're currently running Kernel Mode */
	Benchmark_printLaunchingTechnique (isPrintingProcess, KERNEL_MODE);
	
	/* Loads the Benchmark function */
	dl_bench = Benchmark_loadBenchmarkFunction (desc, isPrintingProcess);
	
	/* Load the Allocation functions */
	dl_alloc = Benchmark_loadAllocationFunctions (desc);
	
	dl_verify = Benchmark_loadVerificationFunctions (desc);
	
	/* Load the Evaluations functions */
	for (i = 0; i < nbEvalLibs; i++)
	{
		res[i] = BenchResult_create (meta_repet);
		overhead[i] = BenchResult_create (meta_repet);
		
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
		
		if (dl_eval[i] == NULL)
		{
			return EXIT_FAILURE;
		}
	}
	
	if (dl_bench == NULL || dl_alloc == NULL)
	{
		return EXIT_FAILURE;
	}
	
	benchmarkInitFct = Description_getKernelInitFunction (desc);
	
	/* Allocate dummy array for cache flushes */
	Benchmark_makeDummyArray (desc);

	// Set step minimum
	if (step == 0)
	{
		step = 1;
	}
	
	/* Allocation method : get data and initialize stuff */
	alloc_init = Description_getMyMallocInit (desc);
	alloc_free = Description_getMyFree (desc);
	
	if (alloc_init (desc) == -1)
	{
		Log_output (-1, "Error: could not initiate the allocation system.\n");
		return EXIT_FAILURE;
	}

	/* Resuming system */
	if (resumeInitCounter (&nCurrentVectorSize, desc->temp_values.current_vector_size))
	{
		if (isPrintingProcess)
		{
			Log_output (-1, "Note: Resuming to last --startvector value : %d\n", nCurrentVectorSize);
		}
	}
	
	if (isPrintingProcess)
	{
		Log_output (-1, "Launching Configuration :\n");
		Log_output (-1, "\t- Meta-repetitions : %u\n", meta_repet);
		Log_output (-1, "\t- Repetitions : %u\n", repet);
	}
	
	/* If iterationCount is enabled, then set all the vectors sizes to this value */
	if (iterationCountIsEnabled)
	{
		iterationCountTable = malloc ( nbVectors * sizeof (*iterationCountTable));
		assert (iterationCountTable != NULL);
		
		for (i = 0; i < nbVectors; i++)
		{
			iterationCountTable[i] = iterationCount / maxStride;
		}
	}
	
	/* Overhead sizes table is simply a 0 table */
	for (i = 0; i < nbVectors; i++)
	{
		overheadSizes[i] = 0;
	}
	
	/* Replaces the basename in case of several input kernels */
	if (Description_getKernelFileNamesTabSize (desc) > 1)
	{
		replaceBaseName (desc);
	}

	/* Main Benchmark loop */
	for ( ; nCurrentVectorSize <= end; nCurrentVectorSize += step)
	{
		/* Init correctly the vector sizes if static ones are not defined */
		if (!isNbSizeDefined)
		{
			for (i = 0; i < nbVectors; i++)
			{
				desc->vectorSizes[i] = nCurrentVectorSize;
				desc->vectorSizes[i] /= maxStride;
			}
		}
		
		desc->temp_values.current_vector_size = nCurrentVectorSize; /* saving current vector size (resume system) */
		
		/* Generates the output CSV file and initializes it */
		if (isProcessEvalHandler && isRequestedToMakeFile) {
			outputCsvFile = Benchmark_createOutputFile (desc, currentExecRepet, nCurrentVectorSize);
			if (outputCsvFile == NULL)
			{
				return EXIT_FAILURE;
			}
			Benchmark_initCsv (desc, outputCsvFile);
		}

		/*  -----------------------------------------------------------------------
			ROUTINE A PROPREMENT PARLER DU BENCH POUR UNE TAILLE nCurrentVectorSize
			----------------------------------------------------------------------- */
		
		/* Initializes the systemState table (having each alignment set) */
		initializeSystemState (desc, systemState);

		vect = Description_getVector(desc, 0);
		
		/* Save current run counters or load them if we're relaunching them */
		saveOrLoadDataFromResuming (&curRuns, systemState, desc, nbVectors, isPrintingProcess);
		
		/* For each alignment process */
		while ((vect == NULL) || systemState[0] <= vect[VSTOP])
		{
			/* Output where we are up to, this is not logged */
			if (isPrintingProcess)
			{
				if (Description_isNbSizeDefined (desc))
				{
					fprintf (stderr, "\r- Benchmark computation process : %3d%% (Mixed sizes)\t\t",(curRuns*100) / (totalRuns));
				}
				else
				{
					fprintf (stderr, "\r- Benchmark computation process : %3d%% (%d/%d)",(curRuns*100) / (totalRuns), nCurrentVectorSize, end);
				}
			}
			
			/* Allocate the vectors */
			allocateArrays (arrays_offset, nbVectors, elemSize, systemState, desc);
			
			/*Overhead computation*/
			/** @todo The overhead calculation seems wrong to me */
			benchmark_kernel (overhead, overheadSizes, arrays_offset, desc, 1);
			
			/* Clear all used vectors */
			for ( i = 0 ; i < nbVectors ; i++ )
			{
				if (benchmarkInitFct != NULL)
				{
					benchmarkInitFct (i, desc->vectorSizes[i], arrays_offset[i], elemSize);
				}
				else
				{
					ClearArray (desc->vectorSizes[i],arrays_offset[i], elemSize);
				}
			}
			
			curRuns++;
	
			/* What is the number of iterations we want to make for each vector ? */
			if (iterationCountIsEnabled)
			{
				iterationSizes = iterationCountTable;
			}
			else
			{
				iterationSizes = desc->vectorSizes;
			}
			benchmark_kernel (res, iterationSizes, arrays_offset, desc, 1);

			/*------------------------------------------------*/
			/* Computing the overhead average for each evaluation library */
			for (evalLoop = 0; evalLoop < nbEvalLibs; evalLoop++)
			{
				overheadAvg[evalLoop] = 0;
			
				/* this lib requested no overhead computation */
				if (!Description_getEvaluationLibraryOverheadFlag(desc, evalLoop))
				{
					continue;
				}
				
				for ( i = 0 ; i < meta_repet ; i++ )
				{
					overheadAvg[evalLoop] += overhead[evalLoop]->time[i];
				}
				overheadAvg[evalLoop] /= meta_repet;
			}
			
			/*Computing all values we wish to use*/
			for ( i = 0 ; i < meta_repet ; i++ )
			{
				for (evalLoop = 0; evalLoop < nbEvalLibs; evalLoop++)
				{
					//Remove overhead 
					res[evalLoop]->time[i] -= overheadAvg[evalLoop];

					switch (Description_getInfoDisplayed (desc, evalLoop)) 
					{
						case ITERATION_COST:
							res[evalLoop]->time[i] = res[evalLoop]->time[i] / (((double) repet) * res[evalLoop]->iterations[i]);
							break;
						case FUNCTION_COST:
							res[evalLoop]->time[i] = res[evalLoop]->time[i] /( (double) repet); 
							break;
						case RAW_NUMBERS:	/* We do nothing */
							break;
						default:
							break;
					}
				}
				
				/* Write in Csv file if we are allowed to do it */
				if (isProcessEvalHandler && isRequestedToMakeFile)
				{
					int problem = NO_ERROR;
					for (evalLoop = 0; evalLoop < nbEvalLibs; evalLoop++)
					{
						if (res[evalLoop]->time[i] < 0)
						{
							problem = OVERHEAD_TOO_HIGH;
							break;
						}
					}
					
					/* Print every eval lib result in the CSV */
					Benchmark_printCsv (res, nbEvalLibs, i, systemState, nbVectors, desc->number_of_resumes, curRuns, outputCsvFile, problem);
				}
			}
			
			/* Computing the next step of alignement possibility*/
			Benchmark_next (desc, systemState);
			memcpy (desc->temp_values.alignments, systemState, nbVectors * sizeof(int));

			desc->temp_values.curruns = curRuns;
			
			/* Save the current counters */
			if (isPrintingProcess)
			{
				resumeSaveCounters(desc);
			}

			/* Free the vectors */
			for (i = 0; i < nbVectors; i++)
			{
				alloc_free (arrays_offset[i]);
			}
			resumeDisableResuming ();
			
			/* If there are no vectors allocated, we only make a single experiment => so we break the loop now */
			if (vect == NULL)
			{
				break;
			}
		}
		
		if (isPrintingProcess)
		{
			resumeSaveCounters (desc);
		}
		
		if (isProcessEvalHandler && isRequestedToMakeFile)
		{
			fclose (outputCsvFile), outputCsvFile = NULL;
		}
	}
	if (isPrintingProcess)
	{
		if (Description_isNbSizeDefined (desc))
		{
			fprintf (stderr, "\r- Benchmark computation process : %3d%% (Mixed sizes)\n",(curRuns*100) / (totalRuns));
		}
		else
		{
			fprintf (stderr, "\r- Benchmark computation process : 100%% (%d/%d)\n", nCurrentVectorSize-step, end);
		}
	}
	
	
	/* Close timer */
	for (i = 0; i < nbEvalLibs; i++)
	{
		close_timer = Description_getEvaluationCloseFunction (desc, i);
		if (close_timer != NULL && isProcessEvalHandler)
		{
			timerCloseRes = close_timer (Description_getEvaluationData (desc, i));
		}
		assert (timerCloseRes == EXIT_SUCCESS);
	}

	/* Call the end function of the alloc library */
	Description_getMyMallocDestroy (desc) ();

	/* Close Dynamic libraries */
	if (Benchmark_closeLibraries (dl_bench, dl_eval, nbEvalLibs, dl_alloc, dl_verify) == -1)
	{
		return EXIT_FAILURE;
	}
	dl_bench = NULL;
	dl_alloc = NULL;
	
	for (i = 0; i < nbEvalLibs; i++) /* Free each evalLib result */
	{
		BenchResult_destroy (res[i]), res[i] = NULL;
		BenchResult_destroy (overhead[i]), overhead[i] = NULL;
	}
	free (res), res = NULL;
	free (overhead), overhead = NULL;
	
	free (systemState), systemState = NULL;
	free (arrays_offset), arrays_offset = NULL;
	free (dl_eval), dl_eval = NULL;
	free (overheadAvg), overheadAvg = NULL;
	if (iterationCountIsEnabled)
	{
		free (iterationCountTable), iterationCountTable = NULL;
	}
	free (overheadSizes), overheadSizes = NULL;

	return EXIT_SUCCESS;
}		  

void Benchmark_next (SDescription *desc,int *systemState)
{
	int i = Description_getNbVectors (desc) - 1;
	int *curent_vect;
	while (i >= 0)
	{
		curent_vect = Description_getVector (desc, i);
		if ( (systemState[i] + curent_vect[VSTEP] > curent_vect[VSTOP]) && i>0)
		{
			systemState[i] = curent_vect[VSTART];
			i--;
		}
		else
		{
			systemState[i] += curent_vect[VSTEP];
			break;
		}
	}
}

void Benchmark_initCsv (SDescription *desc, FILE *stream) {
	unsigned nbVectors = Description_getNbVectors (desc);
	unsigned nbEvalLibs = Description_getNbEvaluationLibrairies (desc);
	unsigned i;
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		fprintf (stream, "\"Eval '%s'\",", Description_getEvaluationLibraryName (desc, i));
	}
	
	fprintf (stream, "\"Id of current run\",\"Number of resumes\",\"Number of arrays\",");
	for (i = 0; i < nbVectors; i++)
	{
		fprintf (stream, "\"Vector #%d alignment\",", i+1);
	}
	fprintf (stream, "\n");
}

/**
 @todo Find the purpose of the overhead argument
**/
void Benchmark_printCsv (BenchResult **res, unsigned nbEvalLibs, unsigned currentMetaRepet, int* offsets, unsigned nb_offsets, int nb_resumes, int currun, FILE *stream, int problem)
{
	unsigned i;
	
	for (i = 0; i < nbEvalLibs; i++)
	{
		fprintf (stream, "%0.6f,", res[i]->time[currentMetaRepet]);
	}
	
	// id of current run, number of resumes, number of arrays, (offsets)*
	fprintf (stream, "%d,%d,%d,", currun, nb_resumes, nb_offsets);
	for(i = 0; i<nb_offsets; i++)
	{
		fprintf(stream, "%d,", offsets[i]);
	}
	
	/* If an error occured with the benchmark result */
	if (problem != NO_ERROR)
	{
		fprintf (stream, "%d,", problem);
	}
	fprintf(stream, "\n");
	fflush(stream);
}
