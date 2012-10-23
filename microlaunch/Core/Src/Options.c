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
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Benchmark.h"
#include "Config.h"
#include "Description.h"
#include "Log.h"
#include "Options.h"
#include "Resume.h"
#include "Toolkit.h"

static struct option option_list[] = {
	{"summary", 0, 0, 'a'},
	{"alloclib", 1, 0, 'A'},
	{"basename", 1, 0, 'b'},
	{"verifylib", 1, 0, 'B'},
	{"cpupin", 1, 0, 'c'},
	{"config", 1, 0, 'C'},
	{"sizedummy", 1, 0, 'd'},
	{"output-dir", 1, 0, 'D'},
	{"endvector", 1, 0, 'e'},
	{"evallib", 1, 0, 'E'},
	{"kernelfunction", 1, 0, 'f'},
	{"no-eval-stack", 1, 0, 'F'},
	{"output-same-dir", 0, 0, 'g'},
    {"execpath", 1, 0, 'G'},
	{"options", 0, 0, 'h'},
	{"stepvector", 1, 0, 'i'},
	{"info", 1, 0, 'I'},
	{"kernelname", 1, 0, 'k'},
	{"logverbosity", 1, 0, 'l'},
	{"metarepetition", 1, 0, 'm'},
	{"vectorspacing", 1, 0, 'M'},
	{"nbvector", 1, 0, 'n'},
	{"no-output", 0, 0, 'N'},
   	{"log-output", 1, 0, 'o'},
   	{"nbprocess", 1, 0, 'O'},
	{"pagecollide", 1, 0, 'p'},
	{"pagesize", 1, 0, 'P'},
	{"vectsurveyor", 1, 0, 'q'},
	{"executerepetition", 1, 0, 'Q'},
	{"repetition", 1, 0, 'r'},
	{"resume", 0, 0, 'R'},
	{"startvector", 1, 0, 's'},
	{"maxstride", 1, 0, 'S'},
	{"omppath", 1, 0, 't'},
	{"execname", 1, 0, 'T'},
	{"execargs",1, 0, 'u'},
	{"suppress-output", 0, 0, 'U'},
	{"iteration-count", 1, 0, 'v'},
	{"execoutput", 1, 0, 'V'},
	{"initfunction", 1, 0, 'w'},
	{"data-size", 1, 0, 'W'},
	{"nbsizes", 1, 0, 'x'},
	{"no-thread-pin", 0, 0, 'X'},
    {"all-print-out", 0, 0, 'y'},
	{"all-metric-output", 0, 0, 'Y'},
	{"resumeid", 1, 0, 'z'},
	{0, 0, 0, 0}
	};

/**
 * A simple way to add a Microlaunch argument
 *
 * Adding an argument to Microlaunch is quite simple. Anyway, you have to be
 * aware of all that you have to handle to make it work properly with the
 * current system.
 *
 * - This Options.c file : Adding your argument in the section above and
 *  adding it in the Option_handleCommandLine () function
 * - In the config.c file : Adding your argument in the XML parser
 * - In the resume.c file : Adding your argument in resumeSave () and resumeLoad ()
 **/

//Static functions 

/**
  * @brief Transform a character chain into a long
  * @param optarg the character chain
  * @return returns the long version of it
  */
static inline long Option_transformArgument (char *optarg)
{
	char *end;
	long res = strtol (optarg, &end, 0);
	assert (end != optarg && *end != '\n' && *optarg != '\0');
	return res;
}

/**
 * @brief Some arguments must be handled before other, for allocation or specific purpose ; here is their handling
 *
 * @param desc the Description we are about to change to handle the special args
 * @param argc the number of arguments passed to the program
 * @param argv the strings table of arguments passed to the program
 * @return 1 if we have to abort the command line handling, 0 else
 */
static inline int Option_handleSpecialArgs (SDescription *desc, int argc, char *argv[])
{
	int opt;
	long val;

    //By default set resume to 0
    Description_setResumeId (desc, 0);
	
	// First, we have to load resume stuff if any
	optind = 1;
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) { // Is the resume id specified?
		if(opt == 'z')
		{
			val = Option_transformArgument (optarg);
			Description_setResumeId (desc, val);
		}
	}
	
	optind = 1;
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) { // Are we resuming?
		if(opt == 'R')
		{
			if (resumeLoad (desc) == -1 || resumeLoadCounters (desc) == -1) // Failed to resume
			{
				Log_output(-1, "Error: Failed to resume.\n");
				exit(EXIT_FAILURE);
			}
			else	// Resumed successfully
			{
				resumeEnableResuming ();
				Description_setResuming (desc, 1);
				desc->number_of_resumes++;
				Log_output (-1, "Loaded resume values successfully. (%dnth resume)\n", desc->number_of_resumes);
			}

			return 1;
		}
	}
	
	/* Special argument : Config file ; if any, we have to load it before loading other command line arglist */
	optind = 1;
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) { // Check in the command line arglist
		if(opt == 'C')
		{
			Description_setConfigFile (desc, optarg);
			
			/* Then, if any, we have to parse special args before those of the command line */
			if (Config_parseXMLSpecialArgs (desc, desc->configFileName) == -1) {
				Log_output (-1, "An error occured while parsing XML config file.\n");
				exit (EXIT_FAILURE);
			}
		}
	}
	
	/* Special argument : nbVectors : it must be allocated before dealing with anything else
		(calling setNbVectors () do a lot of allocation stuff) */
	optind = 1;
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) { // Check in the command line arglist
		if(opt == 'n')
		{
			val = Option_transformArgument (optarg);
			Description_setNbVectors (desc, val);
		}
	}
	
	/* Special argument : nbEvalLib : it must be allocated before dealing with anything else
		(calling setNbEvalLib () do a lot of allocation stuff) */
	optind = 1;
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) { // Check in the command line arglist
		if(opt == 'E')
		{
			Description_parseEvaluationLibraryInput (desc, optarg);
		}
	}
	
	/* In the end, if anybody defined the nbVector argument, let's define it ourself */
	if (Description_getNbVectors (desc) == DEFAULT_NB_VECTORS)
	{
		Description_setNbVectors (desc, 1);
	}
	
	return 0;
}

/**
 * @brief Arguments handling : All the parameters of the program are handled here (expect from the special arguments which are handled before)
 *
 * @param desc the Description we are about to change to handle the special args
 * @param argc the number of arguments passed to the program
 * @param argv the strings table of arguments passed to the program
 */
static inline void Option_handleArguments (SDescription *desc, int argc, char *argv[])
{
	char *configFileName = Description_getConfigFile (desc);
	long val;
	int opt;
	FILE *f;
	unsigned m_nb_args;
	char **m_argv;
	
	// First, we have to parse the XML config file
	if (configFileName != NULL)
	{
		if (Config_parseXML (desc, configFileName) == -1)
		{
			Log_output (-1, "Error: An error occured while parsing the XML config file.\n");
			exit (EXIT_FAILURE);
		}
	}
	
	// Then, we can now handle other arguments in the command line
	optind = 1;

	/* Then, the final values have to be those of the command line */
	while ((opt = getopt_long (argc, argv, "", option_list, NULL)) != -1) {
		switch (opt) {
		case 'a': // --summary
			Description_enableSummary(desc);
			break;
		case 'A': // --alloclib
			Description_setDynAllocLib(desc, optarg);
			break;
		 case 'b': // --basename
			Description_setBaseName (desc, optarg);
			break;
		 case 'B': // --verifylib
			Description_setVerificationLibraryName (desc, optarg);
			break;
		case 'c': // --cpupin
            parseCPUPinning (desc, optarg);
			break;
		case 'd': // --sizedummy
			val = Option_transformArgument (optarg);
			Description_setDummySize (desc, val);
			break;
		case 'D': // --output-dir
			Description_setOutputPath (desc, optarg);
			break;
		case 'e': // --endvector
			val = Option_transformArgument (optarg);
			Description_setEndVectorSize (desc, val); 
			break;
		case 'f': // --kernelfunction
			Description_setDynamicFunctionName (desc, optarg);
			break;
		case 'F': // --no-eval-stack
			Description_disableEvalStack (desc);
			break;
		case 'g': // --output-same-dir
			Description_setOutputPath (desc, ".");
			break;
        case 'G':
            Description_setExecPath (desc, optarg);
            break;
		case 'i': // --stepvector
			val = Option_transformArgument (optarg);
			Description_setVectorSizeStep (desc, val);
			break;
		case 'I': // --info
			Description_parseInfoDisplayedInput (desc, optarg);
			break;
		case 'k': // --kernelname
			if (isFile (optarg))
			{
				Description_setKernelFileNamesTabSize (desc, 1);
				Description_setKernelFileNameAt (desc, optarg, 0);
			}
			else if (isDirectory (optarg))
			{
				parseDir (optarg, desc);
			}
			else
			{
				Log_output (-1, "Error: Cannot resolve kernel name filetype.\n");
				exit (EXIT_FAILURE);
			}
			Description_setKernelFileName (desc, optarg);
			break;
		case 'l': // --logverbosity
			val = Option_transformArgument (optarg);
			Log_setVerbosity (val);
			break;
		case 'm': // --metarepetition
			val = Option_transformArgument (optarg);
			Description_setMetaRepetition (desc, val);
			break;
		case 'M': // --vectorspacing
			val = Option_transformArgument (optarg);
			Description_setVectorSpacing (desc, val);
			break;
		case 'N': // --no-output
			Description_setPromptOutputCsv (desc, 0);
			break;
		case 'o': // --log-output
			f = fopen (optarg, "w");
			Log_setOutput (f);
			break;
		case 'O': // --nbprocess
			val = Option_transformArgument (optarg);

            if (Description_getNbProcess (desc) > 0 && Description_getNbProcess (desc) != val)
            {
                Log_output (-1, "Error: Number of processes already defined and not the same value\n");
                exit (EXIT_FAILURE);
            }
            else
            {
                if (Description_getNbProcess (desc) < 0)
                {
        			Description_setNbProcess (desc, val);
                }
            }
			break;
		case 'p': // --pagecollide
			val = Option_transformArgument (optarg);
				Description_setNbPagesForCollision (desc, val);
			break;
		case 'P': // --pagesize
			val = Option_transformArgument (optarg);
			Description_setPageSize (desc, val);
			break;
		case 'q': // --vectsurveyor
			parse_vect_surveyor (desc,optarg);
			break;	
		case 'Q': // --executerepetition
			val = Option_transformArgument (optarg);
			Description_setExecuteRepets (desc, val);
			break;
		case 'r': // --repetition
			val = Option_transformArgument (optarg);
			Description_setRepetition (desc, val);
			break;
		case 's': // --startvector
			val = Option_transformArgument (optarg);
			Description_setStartVectorSize (desc, val);
			break;
		case 'S': // --maxstride
			val = Option_transformArgument (optarg);
			Description_setMaxStride (desc, val);
			break;
		case 't': // --omppath
			Description_setOmpPath (desc, optarg);
			break;
		case 'T': // --execname
			Description_setExecFileName (desc, optarg);
			break;
		case 'u': // --execargs
			m_argv = getArgs (desc, optarg, &m_nb_args);
			Description_setExecArgc (desc, m_nb_args);
			Description_setExecArgv (desc, m_argv);
			break;
		case 'U': // --suppress-output
			Description_setSuppressOutput (desc, 1);
			break;
		case 'v': // --iteration-count
			val = Option_transformArgument (optarg);
			Description_setIterationCount (desc, val);
			Description_iterationCountEnable (desc);
			break;
		case 'V': // --execoutput
			Description_setOutputFileStream (desc, optarg);
			break;
		case 'w': // --initfunction
			Description_setKernelInitFunctionName (desc, optarg);
			break;
		case 'W': // --data-size
			/* If user specifies "float", let's allocate sizeof(float) */
			if (strncmp ("float", optarg, STRBUF_MAXLEN) == 0)
			{
				Description_setVectorElementSize (desc, sizeof (float));
			}
			/* If he specifies "double", let's allocate sizeof(double) */
			else if (strncmp ("double", optarg, STRBUF_MAXLEN) == 0)
			{
				Description_setVectorElementSize (desc, sizeof (double));
			}
			/* Else, he must have specified a numeric constant */
			else
			{
				val = Option_transformArgument (optarg);
				Description_setVectorElementSize (desc, val);
			}
			break;
		case 'x': // --nbsizes
			Description_nbSizesEnable (desc);
			parseVectorSizes (desc, optarg);
			break;
		case 'X': // --no-thread-pin
			Description_pinThreadDisable (desc);
			break;
		case 'y': // --all-print-out
			Description_setAllPrintOut (desc, 1);
			break;
		case 'Y': // --all-metric-output
			Description_allProcessOutputEnable (desc);
			break;
		case 'z': // --resumeid
			val = Option_transformArgument (optarg);
			Description_setResumeId (desc, val);
			break;
		default:
			assert (argv != NULL && argv[0] != 0);
			break;
		}
	}
}

int Option_handleHelpRequest (int argc, char **argv)
{
	int opt;
	
	/* if ./microlaunch call */
	if (argc == 1)
	{
		Option_printOptions (argv[0]);
		return 0;
	}

	/* if ./microlaunch -h call */
	while ((opt = getopt_long (argc, argv, "h", option_list, NULL)) != -1) {
		switch (opt) {
			case 'h':
				Option_printOptions (argv[0]);
				return 0;
			default: /* We do nothing */
				break;
		}
	}

	return 1;
}

void Option_handleCommandLine (SDescription *desc, int argc, char **argv)
{
	/* Special args handling (nbVector, config file, resume...) */
	if (Option_handleSpecialArgs (desc, argc, argv) == 1)
	{
		return; // We return here if handleSpecialArgs tell us so
	}
	
	/* Normal args handling */
	Option_handleArguments (desc, argc, argv);
}

void Option_printOptions (char *name)
{
	FILE *output = stderr;
	unsigned int i;

	static const char *text[] = 	{
		"\033[1m KERNEL MODE\n*************\033[0m\n",
		"- \033[4mGlobal Arguments\033[0m\n",
		"\t--kernelname <value> : Change the kernel library to be used\n",
		"\t--kernelfunction <value> : Change the kernel function to be used\n",
		"\t--startvector <value> : Sets the vector size (in elements)\n",
		"- \033[4mOptional Arguments\033[0m\n",
		"\t--endvector <value> : Sets the end vector size (in elements)\n",
		"\t--stepvector <value> : Sets the step between each vector size computation\n",
		"\t--data-size <value> : Sets the size of each vector(s) elements (float, double or customed numeric value)\n",
		"\t--repetition <value> : Change the number of repetition to execute\n",
		"\t--metarepetition <value> : Change the number of meta-repetition to execute\n",
		"\t--executerepetition <value> : Change the number of Microlauncher executions to be done\n",
		"\t--initfunction <value> : Sets the function to initialize arrays in the input kernel file\n",
		"\t--cpupin <value> : Change the processor we wish to be pinned on\n",
		"\t--basename <value> : Add a meaningfull name for the output files.\n",
		"\t--maxstride <value> : Change value of the maximum stride\n",
		"\t--alloclib <path> : Select the allocation library\n",
		"\t--evallib \"lib1;lib2;...\" : select evaluation library\n",
		"\t--nbvector <value[1,8]> : Change the maximum of allocated vectors\n",
		"\t--no-output : Disable the output csv file creation\n",
		"\t--vectsurveyor \"{(start,stop,step);...}\": Change the alignment of allocated vectors\n",
		"\t--vectorspacing <value> : change the space (in octet) between two consecutive allocated vector\n",
		"\t--omppath <value> : enables the OpenMP mode and sets the OMP library path (typically /usr/lib)\n",
		"\t--output-dir <value> : Change the directory where output files will be stored.\n",
		"\t--output-same-dir : Output files will be stored in the current directory.\n",
		"\t--resume : Resumes a cancelled run\n",
		"\t--resumeid : Changes the integer suffix for resume data files\n",
		"\t--iteration-count <value> : Set a fixed number of iterations of the benchmark loop\n",
		"\t--nbsizes \"{size1,size2,size3,...}\" : Set a customed size for each vector\n",
		"\t--info \"type1;type2;...\" : Change the information in the output file\n",
		"\t--no-eval-stack : Tells microlaunch to not reverse the stop evaluation library function call order\n",
		"\t--sizedummy <value> : Change the size of the dummy array used to flush caches\n",
		"\t--log-output <value> : Redirect the Microlaunch log output in a file\n",
		"\t--summary : Gives a summary of the execution with several information dealing with the current files in the output directory.\n",
		"\t--logverbosity <value> : Change the Microlaunch log verbosity\n",
		"\t--config <value> : Set a XML configuration file\n",
		"- \033[4mMulti-process Arguments\033[0m\n",
		"\t--kernelnames <value> : file containing the path of the benchmarks\n",
		"\t--nbprocess <value> : number of benchmark process you want to launch\n",
		"\t--all-metric-output : Make all the processes defines by --nbprocess generate an output file\n\n",
		//"- \033[4mUntested Arguments\033[0m\n",
		"\033[1m STAND-ALONE EXECUTION MODE\n****************************\033[0m\n",
		"- \033[4mGlobal Arguments\033[0m\n",
		"\t--execname <value> : Change the executable file to be used\n",
		"\t--execpath <value> : Change the executable path with multiple processes\n",
		"- \033[4mOptional Arguments\033[0m\n",
		"\t--execargs <value> : Add your executable arguments (must be between quotes)\n",
		"\t--repetition <value> : Change the number of repetition to execute\n",
		"\t--metarepetition <value> : Change the number of meta-repetition to execute\n",
		"\t--executerepetition <value> : Change the number of Microlauncher executions to be done\n",
		"\t--execoutput <value> : Redirect the executable output in a file (stdout and stderr redirected)\n",
		"\t--suppress-output : The input executable doesn't print anything\n",
		"\t--no-thread-pin : If the program is using pthreads, Microlaunch pins them by default. This option disable this feature.\n",
		"\t--log-output <value> : Redirect the Microlaunch log output in a file\n",
		"\t--logverbosity <value> : Change the Microlaunch log verbosity\n",
		"\t--config <value> : Set a XML configuration file\n",
		"- \033[4mMulti-process Arguments\033[0m\n",
		"\t--nbprocess <value> : number of benchmark process you want to launch\n",
		"\t--all-metric-output : Make all the processes defines by --nbprocess generate an output file\n",
	};

	fprintf (output, "Usage of options for %s:\n", name);

	for (i = 0; i < sizeof (text) / sizeof (*text); i++)
	{
		fprintf (output, "%s", text[i]);
	}
}
