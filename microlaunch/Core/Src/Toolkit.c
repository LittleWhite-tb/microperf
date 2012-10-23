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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "Defines.h"
#include "Description.h"
#include "Log.h"
#include "SleepTight.h"
#include "Toolkit.h"

int 
getCacheSize (void)
{
    int res;
    char buf[] = "grep 'cache size' /proc/cpuinfo | awk -F : '{print $2}' |awk -F \" \" '{print $1}'";
    char *ptr, *end;
    int status;
    char buffer[STRBUF_MAXLEN];
    
    FILE *output = popen(buf, "r");
    if(output == NULL)
    {
		Log_output (-1, "%s\n", buf);
    	perror ("Error: Opening cache size system call :");
        exit (EXIT_FAILURE);
    }
    
	ptr = fgets (buffer, sizeof (buffer), output);
    
    assert (ptr != NULL);
    status = pclose(output);
    
    if (status == -1)
    {
		Log_output (-1, "%s\n", buf);
    	perror ("Error: Closing cache size system call :");
    	exit (EXIT_FAILURE);
    }
    
    res = strtol (buffer, &end, 10);
    
    if (buffer == end)
    {
    	Log_output (-1, "Error: In getCacheSize function strtol call failed.\n");
    	exit (EXIT_FAILURE);
    }
    
    return res;
}

/**@todo Check triplet_splitter and parse_vect_surveyor */

/*
 *  triplet_splitter, this function takes an strings formatted as a triplet i.e. ([0-9]+,[0,9]+,[0-9]+)
 *  Extracted values (first, second and last fields) of the triplet are stored in an 3-int array
 *  passed as the second argument.
 */
void 
triplet_splitter (char * triplet, int *three_elt_array)
{
    int i;
    char* working_buffer;
    char *ptr, *end;

    /** @todo Why 16? Figure it out */
    ptr = malloc (16 + strlen(triplet));
	working_buffer = ptr;
    strcpy(working_buffer,triplet);

#ifdef DEBUG 
    Log_output (1, "TripletSplitter: Triplet to split: %s \n", triplet);
#endif
    for(i=0 ; i < 3; i++)
    {
        char* end_value;
        long current_value;

        if(i != 2)
        {
            end_value = strchr (working_buffer,',');
        }
        else
        {
            end_value = end_value + strlen (working_buffer);
        }

        current_value = strtol (working_buffer, &end, 10);
        
        /* tripplet scheme : (from, to, step)
        	If to is superior than from, then exit failure.
         */
        if (i == 1 && three_elt_array[0] > current_value)
        {
        	Log_output (-1, "Error: Vector surveyor cannot have a maximum value inferior to the initial value. See microlaunch -h for more details.\n");
        	exit (EXIT_FAILURE);
        }
        
        if (working_buffer == end)
        {
        	Log_output (-1, "Error: In vector surveyor parser strtol call failed.\n");
        	exit (EXIT_FAILURE);
        }
        
#ifdef DEBUG
        Log_output (1, "TripletSplitter: From %s extracted value is: %d \n",working_buffer, current_value);
#endif
        three_elt_array[i] = current_value;

        if (i != 2)
        {
            working_buffer=end_value+1;
        }
    }
    
    free (ptr), ptr = NULL;
}

/*
 * parse_vect_surveyor, this function read the somehow complex character strings VECT_SURVEYOR,
 * and defines the way the vector space is going to be parsed.
 * The VECT_SURVEYOR strings has to follow the format {([0-9]+,[0,9]+,[0-9]+)(;([0-9]+,[0,9]+,[0-9]+))*}
 */
void
parse_vect_surveyor(SDescription *desc, char * VECT_SURVEYOR)
{
    int *vect_descriptor;  // data structure to store vectors description
    char *current_triplet = malloc(1 + strlen(VECT_SURVEYOR));
    char *start_triplet, *stop_triplet;
    int nb_vectors = 0;
    char number[11] = "0123456789";
	int cntVector = Description_getNbVectors (desc);
	Description_setNbVectors (desc, cntVector);
	
    char *ptr = strdup (VECT_SURVEYOR);
    char *working_buffer = ptr;
    while((nb_vectors <cntVector) && ((start_triplet=strpbrk(working_buffer,number))!=NULL))
    {
        vect_descriptor = Description_getVector (desc, nb_vectors);
        /*
         * Detect the begining of a triplet
         */
        stop_triplet=strchr(working_buffer,')');
        strncpy(current_triplet, start_triplet, stop_triplet - start_triplet);
        /*
         *  Adding the \0 is mandatory to avoid some strings overwritting troubles.
         */
        current_triplet[stop_triplet - start_triplet]='\0';
        working_buffer=stop_triplet+1;

        triplet_splitter(current_triplet, vect_descriptor);
        nb_vectors++;
    }
	
	free (current_triplet), current_triplet = NULL;
    free (ptr), ptr = NULL;
}

/**
 * @brief Parse a vector of numbers and call callback
 * @param src the source character chain
 * @param expected expected values
 * @param call the callback
 * @param data the data to pass to the callback with the index and the value
 */
static void parseIt (char *src, unsigned int expected, void (*call) (void*, int, int), void* data) {
	char *end, *strtol_end = NULL;
	unsigned currentVector = 0;
	long res;
	
	// Assert if the first character is not an "{"
	assert (src[0] == '{');
	src++;
	currentVector = 0;
	
	while (((end = strpbrk (src, ",}")) != NULL) && currentVector < expected)
	{
		/* Sets the end to 0, then we can use strtol */
		*end = '\0';
		res = strtol (src, &strtol_end, 10);
		
		if (src == strtol_end)
		{
			Log_output (-1, "Error: in nbSizes parser strtol call failed.\n");
			exit (EXIT_FAILURE);
		}
		
        call (data, currentVector, res);
		
		/* Sets the src to the next element and redo the same job */
		src = end+1;
		currentVector++;
	}
	
	/* The number of effectively read vectors has to be the same as expected value */
	assert (currentVector == expected && end == NULL);
}

/**
 * @brief Handle vector size
 * @param data the Description
 * @param idx the index
 * @param val the value
 */
static void handleVectorSize (void *data, int idx, int val)
{
    SDescription *desc = data;
    Description_setVectorSizeAt (desc, val, idx);
}

void parseVectorSizes (SDescription *desc, char *src) {
	char *ptr, *end;
	unsigned currentVector = 0;
	
	assert (desc != NULL && src != NULL);
	ptr = src;
	unsigned nbVectors = Description_getNbVectors (desc);
	
	// Assert if the first character is not an "{"
	assert (ptr[0] == '{');
	ptr++;
	
	if (nbVectors == 1)
	{
		while (((end = strpbrk (ptr, ",}")) != NULL))
		{
			ptr = end+1;
			currentVector++;
		}

		if (currentVector > 1)
		{
			Log_output (-1, "Warning: Defining automatically %d vectors...\n", currentVector);
			Description_setNbVectors (desc, currentVector);
			nbVectors = currentVector;
		}
	}
	
    parseIt (src, nbVectors, handleVectorSize, desc);
}

/**
 * @brief Handle pinning vector 
 * @param data the Description
 * @param idx the index
 * @param val the value
 */
static void handlePinning (void *data, int idx, int val)
{
    SDescription *desc = data;
    Description_setPinningAt (desc, val, idx);
}

void parseCPUPinning (SDescription *desc, char *src)
{
	char *ptr, *end;
	unsigned currentVector = 0;
	
	assert (desc != NULL && src != NULL);
	ptr = src;
	int nbProcess = Description_getNbProcess (desc);
	
	// Assert if the first character is not an "{"
	assert (ptr[0] == '{');
	ptr++;
	
    while (((end = strpbrk (ptr, ",}")) != NULL))
    {
        ptr = end+1;
        currentVector++;
    }

    if (nbProcess <= 0)
    {
        Description_setNbProcess (desc, currentVector);
        nbProcess = currentVector;
    }

    parseIt (src, nbProcess, handlePinning, desc);
}

unsigned long minUL (unsigned long *tab, size_t size)
{
	unsigned i;
	unsigned long min;
	assert (tab != NULL);
	assert (size != 0);
	
	min = tab[0];
	for (i = 1; i < size; i++)
	{
		if (tab[i] < min)
		{
			min = tab[i];
		}
	}
	
	return min;
}

int getMemorySpaceToAllocated(SDescription *desc)
{
	int vectorSizeOctet = Description_getEndVectorSize(desc) * Description_getVectorElementSize (desc);
	int vectorSpacing = Description_getVectorSpacing(desc);
	int nbVector = Description_getNbVectors(desc);
	
	int memorySpace = vectorSizeOctet*nbVector+(nbVector-1)*vectorSpacing;
	return memorySpace;
}

/**
 * @todo find the purpose of the argument "desc"...
**/
void barrierS (int pipeSF[2], int pipeFS[2])
{
	/* Tell the father I'm ready to go */
	Pipe_childWrite (pipeSF, getpid ());
	
	/* Wait for the Father to tell me I can go */
	Pipe_childRead (pipeFS, NULL);
}

void barrierF (SPipe *pipes, unsigned nbprocess)
{
	unsigned i;
	
	/* Read nbprocess times from the child processes */
	for (i = 0; i < nbprocess ; i++)
	{
		Pipe_fatherRead ( pipes[i].sf, NULL );
	}

	if(sleep_tight() == 1)
	{
		// TODO: stuff to do if we slept
	}
	
	/* Then, tell them all they can go */
	for (i = 0; i < nbprocess; i++)
	{
		Pipe_fatherWrite ( pipes[i].fs, getpid () );
	}
}

int getNbProcessorsAvailable ( void )
{
	FILE *output;
	long nbProcessorsAvailable;
	char buf[3]; /* Max processors = 999 */
	char cmd[] = "grep \"CPU\" /proc/cpuinfo | wc -l";
	int status;
	char *ptr, *end;
	
	output = popen (cmd, "r");
	
	if (output == NULL)
	{
		Log_output (-1, "%s\n", cmd);
		perror ("Error: Getting processors number : ");
		exit (EXIT_FAILURE);
	}
	
    ptr = fgets (buf, sizeof (buf), output);
    assert(ptr != NULL);
    
    status = pclose(output);
    
    if (status == -1)
    {
		Log_output (-1, "%s\n", buf);
    	perror ("Error: Closing processors number system call :");
    	exit (EXIT_FAILURE);
    }
    
    nbProcessorsAvailable = strtol (buf, &end, 10);
    if (buf == end)
    {
    	Log_output (-1, "Error: in getNbProcessorsAvailable strtol call failed.\n");
    	exit (EXIT_FAILURE);
    }
    
    return nbProcessorsAvailable;
}

int isUserRoot()
{
	char *usr = getenv("USER");
	assert (usr != NULL);
	return (strstr ("root", usr) != NULL);
}

/**
 * @brief Handler for any signal we wish to capture
 * @param signal which signal made us arrive here
 * @param info the information about the signal
 * @param data additional information
 */
void SignalHandler_microlauncher (int signal, siginfo_t *info, void *data)
{
	if (signal != SIGUSR1)
	{
		/* Classical signal */
		psignal (signal, "\rInfo: Microlauncher received a signal ");
	}
    else
   	{
		/* Customed signal : One child received a fatal signal, must kill other children */
		if (kill (-getpid(), SIGKILL) == -1)
		{
			perror ("Error: killing child processes");
		}
	}
    
    /* Additional info in case of SIGSEGV signal receiving */
	if (signal == SIGSEGV)
	{
		Log_output (-1, "\t- Address of faulting instruction : %p\n", info->si_addr);
	}
	
	exit (EXIT_FAILURE);

    (void) data;
}

void SignalHandler_child (int signal, siginfo_t *info, void *data)
{
	char buf[STRBUF_MAXLEN];
	unsigned size;
	pid_t father = getppid();
	
	size = snprintf (buf, sizeof (buf), "\rInfo: Child #%d received a signal", getpid());
	assert (size < sizeof(buf));
	
	psignal (signal, buf);
    
	if (signal == SIGSEGV)
	{
		Log_output (-1, "\t- Faulting address : %p\n", info->si_addr);
	}
	
	/* We want to kill the father if child ends */
	if (father != 1)
	{
		if (kill (father, SIGUSR1) == -1)
		{
			/* if errno == ESRCH, it's because another process already murders its own father... >D */
			if (errno != ESRCH)
			{
				char buf[STRBUF_MAXLEN];
				unsigned size;
			
				size = snprintf (buf, sizeof (buf), "Error: An error occured while sending SIGUSR1 to father #%d", father);
				assert (size < sizeof (buf));
				perror (buf);
			}
		}
	}
	
	exit (EXIT_FAILURE);

    (void) data;
}

void SignalHandler_launchingBenchmark (int signal, siginfo_t *info, void *data)
{
	char buf[STRBUF_MAXLEN];
	unsigned size;
	pid_t father = getppid();
	
	size = snprintf (buf, sizeof (buf), "\nError: Launching benchmark function received a signal (from child #%d)", getpid());
	assert (size < sizeof (buf));
	
	psignal (signal, buf);
    
	if (signal == SIGSEGV)
	{
		Log_output (-1, "\t- Address of faulting instruction : %p\n", info->si_addr);
	}
	
	/* We want to kill the father if child ends */
	if (father != 1)
	{
		if (kill (father, SIGUSR1) == -1)
		{
			/* if errno == ESRCH, it's because another process already murders its own father... >D */
			if (errno != ESRCH)
			{
				char buf[STRBUF_MAXLEN];
				unsigned size;
				
				size = snprintf (buf, sizeof (buf), "Error: An error occured while sending SIGUSR1 to father #%d", father);
				assert (size < sizeof (buf));
				perror (buf);
			}
		}
	}
	
	exit (EXIT_FAILURE);

    (void) data;
}

void SignalHandler_benchmark (int signal, siginfo_t *info, void *data)
{
	char buf[STRBUF_MAXLEN];
	unsigned size;
	pid_t father = getppid();
	
	size = snprintf (buf, sizeof (buf), "\nError: Input benchmark received a signal (from Microlauncher child #%d)", getpid());
	assert (size < sizeof (buf));
	
	psignal (signal, buf);
    
	if (signal == SIGSEGV)
	{
		Log_output (-1, "\t- Address of faulting instruction : %p\n", info->si_addr);
	}
	
	/* We want to kill the father if child ends */
	if (father != 1)
	{
		if (kill (father, SIGUSR1) == -1)
		{
			/* if errno == ESRCH, it's because another process already murders its own father... >D */
			if (errno != ESRCH)
			{
				char buf[STRBUF_MAXLEN];
				unsigned size;
			
				size = snprintf (buf, sizeof (buf), "Error: An error occured while sending SIGUSR1 to father #%d", father);
				assert (size < sizeof (buf));
				perror (buf);
			}
		}
	}
	
	exit (EXIT_FAILURE);

    (void) data;
}

unsigned benchmarkIterationsNumber( struct sDescription *desc, int done)
{
	unsigned res;
	
	/* ITERATIONS COMPUTATION */
	res = 2; /* 1 for the overhead, 1 for the real benchmark */
	res *= Description_getMetaRepetition (desc); /* Number of meta-repetitions */
	
	/* The following are done only if we deal with functions and kernel execution, ie not for executables */
	if(Description_getSourceType (desc) != EXECUTABLE_FILE)
	{
		res *= Description_getExperimentNumber (desc) - done;
	}
	
	return res;
}

int isFile (const char *source)
{
	struct stat st;
	assert (source != NULL);
	
	if (stat (source, &st) == -1)
	{
		Log_output (-1, "Error: %s :\n", source);
		perror ("");
		abort ();
	}
	
	return S_ISREG (st.st_mode);
}

int isDirectory (const char *source)
{
	struct stat st;
	assert (source != NULL);
	
	if (stat (source, &st) == -1)
	{
		Log_output (-1, "Error: %s :\n", source);
		perror ("");
		abort ();
	}
	
	return S_ISDIR (st.st_mode);
}

void parseDir (const char *source, SDescription *desc)
{
	assert (source != NULL && desc != NULL);
	DIR *dir = opendir (source);
	struct dirent *dirEntry;
	unsigned dirSize = 0;
	unsigned i;
	char filePath[STRBUF_MAXLEN];
	size_t w_size;
	
	if (dir == NULL)
	{
		Log_output (-1, "Cannot open directory %s :\n", source);
		perror ("");
		abort ();
	}
	
	/* How many files do we have here ? */
	while ((dirEntry = readdir (dir)) != NULL) {
		if (dirEntry->d_name[0] != '.') /* If the file is not hidden */
		{
			dirSize++;
		}
	}
	
	if (closedir (dir) == -1)
	{
		perror ("Error closing directory");
		abort ();
	}
	
	Description_setKernelFileNamesTabSize (desc, dirSize);
	
	dir = opendir (source);
	if (dir == NULL)
	{
		Log_output (-1, "Cannot open directory %s :\n", source);
		perror ("");
		abort ();
	}
	
	/* How many files do we have here ? */
	i = 0;
	while ((dirEntry = readdir (dir)) != NULL) {
		if (dirEntry->d_name[0] != '.') /* If the file is not hidden */
		{
			w_size = snprintf (filePath, sizeof (filePath), "%s/%s", source, dirEntry->d_name);
			assert (w_size < sizeof (filePath));
			Description_setKernelFileNameAt (desc, filePath, i);
			i++;
		}
	}
	
}

char **getArgs ( SDescription *desc, char *pbase, unsigned *argc )
{
	char **argv;
	unsigned i = 0;
	unsigned str_max_len = 2048;
	unsigned argv_max_len = 512;
	unsigned bsize = strnlen (pbase, str_max_len);
	char *ptr_deb;
	
	char *base = strndup (pbase, str_max_len);
	*argc = 0;

	if (bsize >= str_max_len)
	{
		Log_output (-1, "Args list is too long, %d characters max allowed, sorry...\n", str_max_len);
		assert(bsize < str_max_len);
	}
	
	/* Memory allocation */
	argv = malloc ( argv_max_len * sizeof (*argv));
	assert (argv != NULL);
	memset (argv, 0, argv_max_len * sizeof (*argv));
	
	/* First argument must be the executable name */
	argv[0] = strndup (Description_getExecFileName (desc), str_max_len);
	(*argc)++;
	
	ptr_deb = base;
	while (base[i] == ' ') i++; /* Eliminates all unwanted spaces */
	while (base[i] != '\0')
	{
		if(base[i] == ' ')
		{
			base[i] = '\0';
			argv[*argc] = strdup (ptr_deb);
			i++;
			while (base[i] == ' ') i++; /* Eliminates all unwanted spaces */
			ptr_deb = base+i;
			(*argc)++;
			
			if(*argc > argv_max_len)
			{
				Log_output (-1, "Args list cannot have more than %d arguments at the moment, sorry...\n", argv_max_len);
				assert(*argc > argv_max_len);
			}
		}
		i++;
	}
	
	/* If it remains one argument not copied yet */
	if(ptr_deb != &(base[i]))
	{
		argv[(*argc)] = strdup (ptr_deb);
		(*argc)++;
	}
	
	free (base), base = NULL;
	
	return argv;
}

void freeArgs ( unsigned argc, char **argv )
{
	unsigned i;
	
	for (i = 0; i < argc; i++)
	{
		free (argv[i]), argv[i] = NULL;
	}
	free (argv), argv = NULL;
}

void compileInputFile (SDescription *desc)
{
	assert (desc != NULL);
	const Source_type sc = Description_getSourceType (desc);
    char *kernelName = Description_getKernelFileName (desc);
    char *execName = Description_getExecFileName (desc);
    unsigned w_size;
	char buf[STRBUF_MAXLEN];
    char name[] = "/tmp/microXXXXXX";
    int res;
    
    /* EXECUTION MODE */
    if (execName != NULL && sc == EXECUTABLE_FILE)
    {
    	/* If it is a executable source, then let's compile it */
    	if (strstr (execName, ".c"))
    	{
        	res = mkstemp (name);
		    if (res == -1)
		    {
		    	perror ("Error: Cannot create temporary file for compilation ");
		    	exit (EXIT_FAILURE);
		    }
        	close (res); /* Because mkstemp opens the file and we don't need it */
        	
        	if (desc->ompPath != NULL) /* OMP Mode */
		    {
		    	w_size = snprintf (buf, sizeof (buf), "gcc %s -o %s -fopenmp -O3 -Wall -Wextra",
								execName, name);
		   		Log_output (-1, "Compiling in OpenMP mode :\n%s\n", buf);
		    }
		    else /* NORMAL Mode */
		    {
		    	w_size = snprintf (buf, sizeof (buf), "gcc %s -o %s -O3 -Wall -Wextra",
								execName, name);
		   		Log_output (-1, "Compiling in Normal mode :\n%s\n", buf);
		    }
		    assert (w_size < sizeof (buf));

			/* GCC compilation system call */
		    res = system (buf);
		    
        	/* Checking compilation errors */
		    if (res != EXIT_SUCCESS)
		    {
		    	if (WIFSIGNALED (res))
		    	{
		    		w_size = snprintf (buf, sizeof (buf), "Benchmark \"%s\" compilation error :", execName);
		   			assert (w_size < sizeof (buf));
		    		psignal (WTERMSIG (res), buf);
		    	}
		    	
		    	if (WEXITSTATUS (res) == EXIT_FAILURE)
		    	{
		    		Log_output (-1, "Error: \"%s\" compilation failed.\n", execName);
		    	}
		    	exit (EXIT_FAILURE);
		    }
		    
		    /* Replace the existing .c file by the tmp executable filename */
			Description_setExecFileName (desc, name);
    	}
    	return;
    }
	
	/* KERNEL MODE */
	if (kernelName != NULL && (sc == SOURCE_FILE || sc == ASSEMBLY_FILE || sc == OBJECT_FILE))
    {
        // Ok I hate doing this but here we must
        res = mkstemp (name);
        if (res == -1)
        {
        	perror ("Error: Cannot create temporary file for compilation ");
        	exit (EXIT_FAILURE);
        }
        close (res); /* Because mkstemp opens the file and we don't need it */
		
        if (desc->ompPath != NULL) /* OMP Mode */
        {
        	w_size = snprintf (buf, sizeof (buf), "gcc -fopenmp -fPIC -O3 -Wall -Wextra -shared -Wl,-soname,%s -o %s %s %s/libgomp.so.1",
                                    name, name, kernelName, desc->ompPath);
       		Log_output (-1, "Compiling in OpenMP mode :\n%s\n", buf);
        }
        else /* NORMAL Mode */
        {
        	w_size = snprintf (buf, sizeof (buf), "gcc -fPIC -O3 -Wall -Wextra -shared -Wl,-soname,%s -o %s %s",
                                    name, name, kernelName);
       		Log_output (-1, "Compiling in Normal mode :\n%s\n", buf);
        }
        assert (w_size < sizeof (buf));

		/* GCC compilation system call */
        res = system (buf);
        
        /* Checking compilation errors */
        if (res != EXIT_SUCCESS)
        {
        	if (WIFSIGNALED (res))
        	{
        		w_size = snprintf (buf, sizeof (buf), "Benchmark \"%s\" compilation error :", kernelName);
        		assert (w_size < sizeof (buf));
        		psignal (WTERMSIG (res), buf);
        	}
        	
        	if (WEXITSTATUS (res) == EXIT_FAILURE)
        	{
        		Log_output (-1, "Error: \"%s\" compilation failed.\n", kernelName);
        	}
        	exit (EXIT_FAILURE);
        }

        Description_setDynamicLibraryName (desc, name, 1);
    }
}

char *strDuplicate (const char *src, size_t size) {
	char *res;
	size_t length;
	
	// Paranoid, check if source string not NULL
	if (src == NULL)
    {
        return NULL;
    }
	
	/* If the length of the input string is equal to the limit size given,
		then we cannot continue */
	length = strnlen (src, size);
	if (size == length || src[length] != '\0')
	{
		Log_output (-1, "Error: String buffer max size (given : %u) is not high enough.\n", size);
		exit (EXIT_FAILURE);
	}
	
	/* Real string duplication and check for its result */
	res = strndup (src, length);
	if (res == NULL)
	{
		perror ("Cannot allocate string");
		exit (EXIT_FAILURE);
	}
	
	return res;
}

int generateSummary (SDescription *desc)
{
	assert (desc != NULL);
	unsigned metaRepetitions = Description_getMetaRepetition (desc);
	unsigned executeRepetitions = Description_getExecuteRepets (desc);
	unsigned nbProcessOutput = Description_getNbProcess (desc);
	int allProcessOutput = Description_isAllProcessOutputEnabled (desc);
	char *outputDir = Description_getOutputPath (desc);
	int ret, w_size;
	char cmdBuf[STRBUF_MAXLEN];
	
	if (!allProcessOutput)
	{
		nbProcessOutput = 1;
	}
	
	/* CSV creation */
	if (chdir ("utils/summarycreator/") == -1)
	{
		perror ("Error changing directory 1");
		exit (EXIT_FAILURE);
	}
	w_size = snprintf (cmdBuf, STRBUF_MAXLEN, "php summaryCreator.php %u %u %u kernel toto ../%s/", nbProcessOutput,
						metaRepetitions, executeRepetitions, outputDir);
	assert (w_size < STRBUF_MAXLEN);
	
	fprintf (stderr, "Generating summary... ");
	ret = system (cmdBuf);
	if (ret != EXIT_SUCCESS)
	{
		Log_output (-1, "\nError: Something went wrong with the summarycreator. Do you have PHP installed on your machine ?\n");
		exit (EXIT_FAILURE);
	}
	
	/* XLS creation */
	w_size = snprintf (cmdBuf, STRBUF_MAXLEN, "php csvToXls.php csv_files/ > ../../%s/summary.xls", outputDir);
	assert (w_size < STRBUF_MAXLEN);
	
	ret = system (cmdBuf);
	assert (ret == EXIT_SUCCESS);
	fprintf (stderr, "done\n");
	
	if (chdir ("../../") == -1)
	{
		perror ("Error changing directory 2");
		exit (EXIT_FAILURE);
	}
	
	return 1;
}

