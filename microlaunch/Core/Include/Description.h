#ifndef H_DESCRIPTION
#define H_DESCRIPTION

#include <stdio.h>
#include <stdint.h>

/**
 * These defines are the initial values of the sDescription structure.
 * They are no default values, these are set in the Description_setDefaults () function
 */
#define DEFAULT_ARRAY_SIZE 2500000
#define DEFAULT_NB_VECTORS -10
#define DEFAULT_META_REPETITION -10
#define DEFAULT_EXEC_REPETITION -10
#define DEFAULT_REPETITION -10
#define DEFAULT_CPU_DEST -10
#define DEFAULT_STEP_VECTOR -10
#define DEFAULT_ELEM_SIZE -10
#define DEFAULT_ITERATION_COUNT 0
#define DEFAULT_VECTOR_SPACING -10
#define DEFAULT_NBPAGESFORCOLLISION -10
#define DEFAULT_DUMMYSIZE -10
#define DEFAULT_MAX_STRIDE -10
#define DEFAULT_PROMPT_OUTPUT -10
#define DEFAULT_INFO_DISPLAYED -10
#define DEFAULT_NBPROC_AVAILABLE -10
#define DEFAULT_EXEC_ARGC -10
#define DEFAULT_NB_PROCESS -10
#define DEFAULT_SUPPRESS_OUTPUT -10
#define DEFAULT_USER_ROOT -10
#define DEFAULT_PAGESIZE -10
#define DEFAULT_RESUMING_VALUE -10
#define DEFAULT_RESUME_NB -10
#define DEFAULT_RESUME_ID -10

struct sDescription; /* See verificationFctInit typedef */

// Define the type for the logistic evaluation functions
typedef void *(*evaluationLogisticFctInit) (void);
typedef int (*evaluationLogisticFctClose) (void *evalData);

// Define the type for the evaluation functions
typedef double (*evaluationFct) (void *evalData);

// Define the type of the flag used by libraries to communicate their will to
// be taken into account when measuring the overhead.
typedef unsigned int evaluationFlag;

// Deinf the type for the veryfing functions
typedef void *(*verificationFctInit) (struct sDescription *desc);
typedef void (*verificationFctDisplay) (void *context, FILE *fp);
typedef void (*verificationFctDestroy) (void *context);

// Define the type for allocation librairies
typedef int (*personalized_malloc_init_t)(void*);
typedef void* (*personalized_malloc_t)(size_t, int, int);
typedef void (*personalized_free_t)(void*);
typedef void (*personalized_malloc_destroy_t)();

// Define the type of program we want to launch : source file, assembly file, library file or executable filename
typedef enum { UNKNOWN_FILE, SOURCE_FILE, ASSEMBLY_FILE, LIBRARY_FILE, OBJECT_FILE, EXECUTABLE_FILE } Source_type;

/**
 * @brief struct sResumeValues define the key resume values we have to store temporarily
 */
typedef struct sResumeValues
{
	int *alignments;		/**< @brief Current alignment set */
	int current_meta_repet;	/**< @brief Current meta-repetition executed in the program */
	int current_vector_size;	/**< @brief Current vector size executed in the program */
	int current_execute_repet;	/**< @brief Current execute repetition executed in the program */
	int curruns;				/**< @brief Current number of runs executed in the program */
} SResumeValues;

/**
  * @brief struct sDescription defines the description information given by the user
  */
typedef struct sDescription
{
    int isRoot;             /**< @brief Is the user root? */
    int *infoDisplayed;      /**< @brief What is the information we want displayed ? (table) */
	
    int startVectorSize;    /**< @brief Define the start vector size */
    int endVectorSize;      /**< @brief Define the end vector size */
    int vectorSizeStep;     /**< @brief Define the vector size step */
    int vectorElementSize;	/**< @brief Define the vector element size in octets (float, double or event customed) */
    unsigned long *vectorSizes;	/**< @brief Define vector customed sizes */
    int isNbSizesDefined; /**< @brief Define whether or not customed sizes are defined */
    
    int iterationCount_Enabled;	/**< @brief Define whether or not the iteration-count value is given by user */
    int iterationCount;		/**< @brief Define the number of iteration the user want to do */

    int repetition;         /**< @brief Define the repetition value */
    int metaRepetition;     /**< @brief Define the meta-repetition value */
    int maxStride;             /**< @brief Define the stride of the experiment */
    int pageSize;           /**< @brief Define the page size */
    char *baseName;         /**< @brief Define the base name of the experiment */

    int nbPagesForCollision; /**< @brief Define the number of pages for a collision */

	int vectorSpacing;       /**< @brief spacing between vectors in octet*/
	int nbprocess;         	/**< @brief number of benchmark process to launch*/
    unsigned long *pinning;	/**< @brief Define the pinning values */
	int nbProcessorsAvailable;	/**< @brief number of processors available on the machine */
    int dummySize;          /**< @brief Define the dummy size */
    double *dummyArrayAligned; /**< @brief Give the dummy array (aligned version) */
    double *dummyArrayNonAligned; /**< @brief Give the dummy array (non aligned version) */
    char *configFileName;			/**< @brief filename of the .cfg config file containing default options */
    int summary;	/**@brief Whether or not we want a summary of the execution files at the end of the program */

    char *dynlibName;       /**< @brief Define the name of the dynamic library */
    char *dynlibFunc;       /**< @brief Define the name of the dynamic function name */
    char *kernelFileName;   /**< @brief Define the name of the kernel file name */
    unsigned nbKernels;		/**< @brief Defines the number of kernels to be launched */
    char **kernelFileNames;	/**< @brief Defines the strings table containing the kernel filenames */
    unsigned kernelCurrentId;		/**< @brief Defines the kernel ID of the table */
    char *kernelInitFunctionName;	/**< @brief Define the name of the benchmark init function */
    char *execFileName;   /**< @brief Define the name of the executable file name */
    char **execArgv;		/**< @brief Define the executable arguments */
    int execArgc;		/**< @brief Define the executable arguments number */
    char *execPath;     /**< @brief Define the executable path */
    int dynlibDelete;       /**< @brief Define whether or not we should delete the dynamic library at the end */
    int experimentNumber;	/**< @brief Define the number of experiments we want to use */

	int *tubeSF;         /**< @brief the pipe from the sons to the father to make the sync*/
	int *tubeFS;		   /**< @brief the pipe from the fathe to the sons to make the sync*/
	char *ompPath;		/**< @brief customed path to OMP library */
	char *outputPath;	/**< @brief customed path to output files storing */
	int suppressOutput;	/**< @brief defines whether or not the output of the input executable is displayed or not */
	char *outputFileStream;	/**< @brief defines the output filestream to be used for the executable output */

    int **vectorDescriptor;    				    /**< @brief Vector description */
    int nbVector;                               /**< @brief Number of vectors */
#define VSTART  0
#define VSTOP   1
#define VSTEP   2
  
    int CPUDest;    /**< @brief Define the CPU destination (for pinning) */
    int cpupinDefined;	/**< @brief Whether or not the cpupin option is defined or not */

    int logVerbosity;   /**< @brief Define the log verbosity requested */
    FILE* logOutput;    /**< @brief Define the log output */
    int promptOutputCsv;     /**< @brief Define if we want an csv output file or not */
    
    int executeRepet;	/**< @brief Define the number of ML executions the user wants */

    void *kernelFunction;   /**< @brief Kernel function */
    void *kernelInitFunction;	/**< @brief Kernel initialization function */

    char **evaluationLibraryName;    /**< @brief Evaluation library name */
    void **evalData;	/**< @brief Evaluation data */
    evaluationLogisticFctInit *evaluationInit;  /**< @brief Evaluation init function */
    evaluationLogisticFctClose *evaluationClose;  /**< @brief Evaluation close function */
    evaluationFct *evaluationStart;  /**< @brief Evaluation start function */
    evaluationFct *evaluationStop;   /**< @brief Evaluation stop function */
    evaluationFlag **evaluationOverheadFlags;	/**< @brief Is the library overhead measured? */
    unsigned nbEvalLibs;	/**< @brief Defines the number of evaluation librairies used in the program */
    int evalStack;			/**< @brief Defines whether or not the evaluation librairies have to be used as a stack or not */
    Source_type sourceType;	/**< @brief Defines the input source type (object, assembly, C, dyn library or stand-alone exec program) */
    
    char *verificationLibraryName;	/**< @brief Defines the verification library name */
    void *verificationContextData;	/**< @brief Defines the prospective context data of the verification library */
    verificationFctInit verificationInit;	/**< @brief Defines the init function pointer of the verification library */
    verificationFctDisplay verificationDisplay;	/**< @brief Defines the display function pointer of the verification library */
    verificationFctDestroy verificationDestroy;	/**< @brief Defines the destroy function pointer of the verification library */

    char *dynAllocLib;	    /**< @brief Defines the name of the dynamic library that will be used for memory allocation */
    personalized_malloc_init_t myMallocInit;	/**< @brief Defines the callback function to initialize the allocation library */
    personalized_malloc_t myMalloc;				/**< @brief Defines the callback function to allocate memory */
    personalized_free_t myFree;					/**< @brief Defines the callback function to release memory */
    personalized_malloc_destroy_t myMallocDestroy;	/**< @brief Defines the callback function to quit the allocation library */

    int resuming;			/**< @brief Whether or not the --resume argument is specified */
    int number_of_resumes;	/**< @brief Number of times the experiment has been resumed */
    int resumeId;			/**< @brief Id of resuming job */
    int threadPin;			/**< @brief Defines whether or not we have to pin threads in exec mode (only) */
    int allProcessOutput;	/**< @brief Defines whether or not every process has to deal with evaluation library */
    int allPrintOut;	    /**< @brief Defines whether or not every child should be printing out */
    int isPrintingProcess;	/**< @brief Defines whether or not the process has to deal with print stuff */
    int isEvalHandlerProcess;	/**< @brief Defines whether or not the process has to deal with output file and evaluation library stuff */
    int processId;					/**< @brief Defines the process identifier */
    char *outputFileName;			/**< @brief Defines the output filename */

    SResumeValues temp_values;		/**< @brief structure containing data representation the current execution state */
} SDescription;

/**
 * @brief Show possible warnings to the user regarding the desc structure
 * @param desc the SDescription we wish to use
 */
void Description_ShowWarnings (SDescription *desc);

/**
 * @brief Set default values for the description
 * @param desc the SDescription we wish to use
 */
void Description_setDefaults (SDescription *desc);

/**
 * @brief Create a SDescription
 * @return An allocated and set to 0 SDescription
 */
SDescription *Description_create (void);

/**
 * @brief Release a SDescription
 * @param desc the SDescription we wish to free
 */
void Description_destroy (SDescription *desc);

/**
 * @brief Set the number of ML executions we want
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setExecuteRepets (SDescription *desc, int value);

/**
 * @brief Get the number of ML executions we want
 * @return the number of ML executions we want
 */
int Description_getExecuteRepets (SDescription *desc);

/**
 * @brief Set the start vector size variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 * @param idx the number of the vector we wish use
 */
void Description_setStartVectorSize (SDescription *desc, int value);

/**
 * @brief Get the startVectorSize variable of the idx vector
 * @return Is the description describing a start vector size execution
 */
int Description_getStartVectorSize (SDescription *desc);

/**
 * @brief Disables the iteration count parameter of the description
 * @param desc the SDescription we wish to use
 */
void Description_iterationCountDisable (SDescription *desc);

/**
 * @brief Enables the iteration count parameter of the description
 * @param desc the SDescription we wish to use
 */
void Description_iterationCountEnable (SDescription *desc);

/**
 * @brief Returns whether or not the iteration count parameter is enabled
 * @param desc the SDescription we wish to use
 * @return Whether or not the iteration count parameter is enabled
 */
int Description_isIterationCountEnabled (SDescription *desc);

/**
 * @brief Sets the iteration-count value inside the description
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_setIterationCount (SDescription *desc, int value);

/**
 * @brief Returns the iteration count parameter
 * @param desc the SDescription we wish to use
 * @return the iteration count parameter
 */
int Description_getIterationCount (SDescription *desc);

/**
 * @brief Set the end vector size variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 * @param idx the number of the vector we wish use
 */
void Description_setEndVectorSize (SDescription *desc, int value);

/**
 * @brief Get the endVectorSize variable of the idx vector
 * @return Is the description describing a end vector size execution
 */
int Description_getEndVectorSize (SDescription *desc);

/**
 * @brief Set the vector size step variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 * @param idx the number of the vector we wish use
 */
void Description_setVectorSizeStep (SDescription *desc, int value);

/**
 * @brief Get the vector size step variable of the idx vector
 * @return Is the description describing a end vector size execution
 */
int Description_getVectorSizeStep (SDescription *desc);

/**
 * @brief Set the number experiments we want to make
 * @param desc the SDescription we wish to use
 * @param value the number of the experiments
 */
void Description_setExperimentNumber (SDescription *desc, int value);

/**
 * @brief Get the number experiments we want to make
 * @return the experiment number
 */
int Description_getExperimentNumber (SDescription *desc);

/**
 * @brief Set the root variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setRoot (SDescription *desc, int value);

/**
 * @brief Get the isRoot variable
 * @return Is the description describing a root execution
 */
int Description_getRoot (SDescription *desc);

/**
 * @brief Set the experience base name variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setBaseName (SDescription *desc, char* value);

/**
 * @brief Get the experience base name variable
 * @return returns the base experience name
 */
char* Description_getBaseName (SDescription *desc);

/**
 * @brief Set the path to the OMP dynamic library location
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setOmpPath (SDescription *desc, const char* value);

/**
 * @brief Get the path to the OMP dynamic library location
 * @return returns it
 */
char* Description_getOmpPath (SDescription *desc);

/**
 * @brief Get the path to the output files storage
 * @return returns it
 */
char* Description_getOutputPath (SDescription *desc);

/**
 * @brief Set the path to the output files storage
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setOutputPath (SDescription *desc, const char* value);

/**
 * @brief Set the autorisation to create an output CSV file
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setPromptOutputCsv(SDescription *desc, int value);

/**
 * @brief Get the autorisation to create an output CSV file
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
int Description_getPromptOutputCsv(SDescription *desc);

/**
 * @brief Set the information displayed variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setInfoDisplayed (SDescription *desc, char *value, unsigned idX);

/**
 * @brief Get the information displayed variable
 * @param desc struct sDescription that is used
 * @return returns what the information we want to display is
 */
int Description_getInfoDisplayed (SDescription *desc, unsigned idX);

/**
 * @brief Read the configuration file
 * @param desc struct sDescription that is used
 * @param szConfigFile Name of the configuration file
 * @return whether or not the read had a problem
 */
int Description_ReadConfigFile (SDescription *desc, char *szConfigFile);

/**
 * @brief Get the maximum stride variable
 * @param desc struct sDescription that is used
 * @return returns the stride
 */
int Description_getMaxStride (SDescription *desc);

/**
 * @brief Set the stride variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setMaxStride (SDescription *desc, int value);


/**
 * @brief Get the vector spacing variable
 * @param desc the SDescription we wish to use
 * @return value of the vector spacing
 */
int Description_getVectorSpacing(SDescription *desc);


/**
 * @brief Set the vector spacing variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setVectorSpacing(SDescription *desc, int value);

/**
 * @brief Get the number of benchmark process variable
 * @param desc the SDescription we wish to use
 * @return value of the number of benchmark process
 */
int Description_getNbProcess (SDescription *desc);

/**
 * @brief Set the number of benchmark process variable
 * @param desc the SDescription we wish to use
 * @param val the value we wish to set
 */
void Description_setNbProcess (SDescription *desc, int val);


/**
 * @brief Set the number of processors available on the machine
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setNbProcessorsAvailable(SDescription *desc, int value);

/**
 * @brief Get the number of processors available on the machine
 * @param desc the SDescription we wish to use
 * @return the number of processors available on the machine
 */
int Description_getNbProcessorsAvailable(SDescription *desc);

/**
 * @brief Get the dummy size variable
 * @param desc struct sDescription that is used
 * @return returns the dummy size
 */
int Description_getDummySize (SDescription *desc);

/**
 * @brief Set the dummy size variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setDummySize (SDescription *desc, int value);

/**
 * @brief Get the aligned dummy array variable
 * @param desc struct sDescription that is used
 * @return returns the dummy array
 */
double *Description_getDummyArrayAligned (SDescription *desc);

/**
 * @brief Set the dummy array variable
 * @param desc the SDescription we wish to use
 * @param aligned aligned pointer we wish to set
 * @param non_aligned non aligned pointer we wish to set
 */
void Description_setDummyArray (SDescription *desc, double* aligned, double* non_aligned);

/**
 * @brief Get the repetition variable
 * @param desc struct sDescription that is used
 * @return returns the repetition
 */
int Description_getRepetition (SDescription *desc);

/**
 * @brief Set the repetition variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setRepetition (SDescription *desc, int value);

/**
 * @brief Get the meta repetition variable
 * @param desc struct sDescription that is used
 * @return returns the meta repetition
 */
int Description_getMetaRepetition (SDescription *desc);

/**
 * @brief Set the meta repetition variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setMetaRepetition (SDescription *desc, int value);

/**
 * @brief Get the page size variable
 * @param desc struct sDescription that is used
 * @return returns the page size
 */
int Description_getPageSize (SDescription *desc);

/**
 * @brief Set the page size variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setPageSize (SDescription *desc, int value);

/**
 * @brief Get the experience name base variable
 * @param desc struct sDescription that is used
 * @return returns the experience name base
 */
char *Description_getExpNameBase (SDescription *desc);

/**
 * @brief Set the experience name base variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setExpNameBase (SDescription *desc, char *value);

/**
 * @brief Get the nbPagesForCollision variable
 * @param desc struct sDescription that is used
 * @return returns the nbCollisionsForPage 
 */
int Description_getNbPagesForCollision (SDescription *desc);

/**
 * @brief Set the nbPagesForCollision variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setNbPagesForCollision (SDescription *desc, int value);

/**
 * @brief Returns vector descriptor
 * @param desc the struct sDescription that describes this execution
 * @param idx the index of the vector we want
 * @return the pointer to the description, can be NULL
 */
int* Description_getVector (SDescription* desc, int idx);

/**
 * @brief Return the number of vectors
 * @param desc struct sDescription that is used
 * @return returns the number of vectors
 */
int Description_getNbVectors (SDescription *desc);

/**
 * @brief Return the number of pinning 
 * @param desc struct sDescription that is used
 * @param idx the index
 * @return returns the pinning value
 */
int Description_getPinningAt (SDescription *desc, int idx);

/**
 * @brief Set the nbVector variable
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setNbVectors (SDescription *desc, int value);

/**
 * @brief Return the name of the dynamic library
 * @param desc the description we wish to use
 * @return returns the name of the dynamic library
 *
 * This function might have to actually create this dynamic library
 */
char *Description_getDynamicLibraryName (SDescription *desc);

/**
 * @brief Returns the name of the dynamic function name we wish to use
 * @param desc the description we wish to use
 * @return returns the name of the dynamic function 
 */
char *Description_getDynamicFunctionName (SDescription *desc);

/**
 * @brief Sets the name of the dynamic function name we wish to use
 * @param desc the description we wish to use
 * @param value the value we wish to set
 */
void Description_setDynamicFunctionName (SDescription *desc, char *value);

/**
 * @brief Sets the name of the dynamic library we wish to use
 * @param desc the description we wish to use
 * @param value the value we wish to set
 * @param shouldDelete should we destroy the file once the program is over
 */
void Description_setDynamicLibraryName (SDescription *desc, char *value, int shouldDelete);

/**
 * @brief Sets the kernel file source
 * @param desc the description we wish to use
 * @param name name of the kernel file source
 */
void Description_setKernelFileName (SDescription *desc, char *name);

/**
 * @brief Return the executable file source
 * @param desc struct sDescription that is used
 * @return name of the executable file source
 */
char *Description_getExecFileName (SDescription *desc);

/**
 * @brief Sets the executable arguments
 * @param desc the description we wish to use
 * @param argv => table of string to be set
 */
void Description_setExecArgv (SDescription *desc, char **argv);

/**
 * @brief Return the executable arguments
 * @param desc struct sDescription that is used
 * @return the executable arguments
 */
char **Description_getExecArgv (SDescription *desc);


/**
 * @brief Sets the executable arguments number
 * @param desc the description we wish to use
 * @param argc number of executable arguments
 */
void Description_setExecArgc (SDescription *desc, int argc);

/**
 * @brief Return the executable arguments number
 * @param desc struct sDescription that is used
 * @return executable arguments number
 */
int Description_getExecArgc (SDescription *desc);

/**
 * @brief Sets the executable file source
 * @param desc the description we wish to use
 * @param name name of the executable file source
 */
void Description_setExecFileName (SDescription *desc, char *name);

/**
 * @brief Return the kernel file source
 * @param desc struct sDescription that is used
 * @return name of the kernel file source
 */
char *Description_getKernelFileName (SDescription *desc);

/**
 * @brief Return the CPU destination used to pin it down
 * @param desc struct sDescription that is used
 * @return returns the CPU destination we wish to be pinned down at, -1 if none was defined
 */
int Description_getCPUDest (SDescription *desc);

/**
 * @brief Set the CPU destination used to pin it down
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setCPUDest (SDescription *desc, int value);

/**
 * @brief Return the tube file descriptor for the process sync between sons and father
 * @param desc struct sDescription that is used
 * @return returns the tube file descriptor used for the sync
 */
int *Description_getTubeSF (SDescription *desc);

/**
 * @brief Set the tube file descriptor used by the current process sync between sons and father
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setTubeSF (SDescription *desc, int* value);

/**
 * @brief Return the tube file descriptor for the process sync between father and sons
 * @param desc struct sDescription that is used
 * @return returns the tube file descriptor used for the sync
 */
int *Description_getTubeFS (SDescription *desc);

/**
 * @brief Set the tube file descriptor used by the current process 
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setTubeFS (SDescription *desc, int* value);

/**
 * @brief Return the Log output
 * @param desc struct sDescription that is used
 * @return returns the log output
 */
FILE* Description_getLogOutput (SDescription *desc);

/**
 * @brief Set the Log output
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setLogOutput (SDescription *desc, FILE* value);

/**
 * @brief Return the log verbosity
 * @param desc struct sDescription that is used
 * @return returns the log verbosity we wish to have
 */
int Description_getLogVerbosity (SDescription *desc);

/**
 * @brief Set the log verbosity
 * @param desc the SDescription we wish to use
 * @param value the value we wish to set
 */
void Description_setLogVerbosity (SDescription *desc, int value);

/**
 * @brief Pin CPU down depending on what is in the description
 * @param desc the SDescription we wish to use
 * @return returns 0 if all is successful, -1 if there is an issue
 */
int Description_pinCPU (SDescription *desc);

/**
 * @brief Set up the log module depending on what was defined
 * @param desc the SDescription we wish to use
 */
void Description_setupLog (SDescription *desc);

/**
 * @brief Set kernel function
 * @param fct the kernel function we wish to use
 * @param desc the SDescription we wish to use
 */
void Description_setKernelFunction (SDescription *desc, void *fct);

/**
 * @brief Get kernel function
 * @param desc the SDescription we wish to use
 * @return the kernel function pointer
 */
void* Description_getKernelFunction (SDescription *desc);
/**
 * @brief Set evaluation initialization function
 * @param fct the kernel function we wish to use
 * @param desc the SDescription we wish to use
 */
void Description_setEvaluationInitFunction (SDescription *desc, evaluationLogisticFctInit fct, unsigned idX);

/**
 * @brief Get the evaluation initialization function
 * @param desc the SDescription we wish to use
 * @return the evaluation initialization function pointer
 */
evaluationLogisticFctInit Description_getEvaluationInitFunction (SDescription *desc, unsigned idX);

/**
 * @brief Set evaluation close function
 * @param fct the kernel function we wish to use
 * @param desc the SDescription we wish to use
 */
void Description_setEvaluationCloseFunction (SDescription *desc, evaluationLogisticFctClose fct, unsigned idX);

/**
 * @brief Get the evaluation close function
 * @param desc the SDescription we wish to use
 * @return the evaluation close function pointer
 */
evaluationLogisticFctClose Description_getEvaluationCloseFunction (SDescription *desc, unsigned idX);

/**
 * @brief Set evaluation start function
 * @param fct the kernel function we wish to use
 * @param desc the SDescription we wish to use
 */
void Description_setEvaluationStartFunction (SDescription *desc, evaluationFct fct, unsigned idX);

/**
 * @brief Get the evaluation start function
 * @param desc the SDescription we wish to use
 * @return the evaluation start function pointer
 */
evaluationFct Description_getEvaluationStartFunction (SDescription *desc, unsigned idX);

/**
 * @brief Set kernel function
 * @param fct the kernel function we wish to use
 * @param desc the SDescription we wish to use
 */
void Description_setEvaluationStopFunction (SDescription *desc, evaluationFct fct, unsigned idX);

/**
 * @brief Get the evaluation start function
 * @param desc the SDescription we wish to use
 * @return the evaluation start function pointer
 */
evaluationFct Description_getEvaluationStopFunction (SDescription *desc, unsigned idX);

/**
 * @brief Returns the name of the dynamic evaluation library name we wish to use
 * @param desc the description we wish to use
 * @return returns the name of the dynamic evaluation library 
 */
char *Description_getEvaluationLibraryName (SDescription *desc, unsigned idX);

/**
 * @brief Sets the name of the dynamic evaluation library name we wish to use
 * @param desc the description we wish to use
 * @param value the value we wish to set
 */
void Description_setEvaluationLibraryName (SDescription *desc, char *value, unsigned idX);

/**
 * @brief Sets the flag defining if an overhead has to be computed for this lib.
 * @param desc the description we wish to use
 * @param value the value we wish to set
 * @param idX the library index
 */
void Description_setEvaluationLibraryOverheadFlag (SDescription *desc, evaluationFlag *ptr, unsigned idX);

/**
 * @brief Gets the flag defining if an overhead has to be computed for this lib.
 * @param desc the description we wish to use
 * @param idX the library index
 * @return The flag value
 */
evaluationFlag Description_getEvaluationLibraryOverheadFlag (SDescription *desc, unsigned idX);

/**
 * @brief Sets the type of source file 
 * @param desc the description we wish to use
 * @param value the value we wish to set
 */
void Description_setSourceType (SDescription *desc, Source_type sc );

/**
 * @brief Returns the type of source file we are using in the program
 * @param desc the description we wish to use
 * @return returns the brief :p
 */
Source_type Description_getSourceType (SDescription *desc);

/**
 * @brief Verification of the values set in the descritpion structure by the parsing of the commande line
 * @param desc the description we wish to use */
int Description_AssertValues (SDescription *desc);

/**
 * @brief Set the configuration file name, if specified in the command line
 * @param desc the description we wish to use
 * @param value the value we wish to set
 */
void Description_setConfigFile (SDescription *desc, char *value);

/**
 * @brief Get the configuration file name
 * @param desc the description we wish to use
 * @return the configuration file name
*/
char *Description_getConfigFile (SDescription *desc);

/**
 * @brief Sets the dynamic allocation library's path
 * @param desc the description we wish to use
 * @param path path of the .so to use*/
void Description_setDynAllocLib(SDescription *desc, char* path);

/**
 * @brief Sets the pointer for the initialization function of our allocation library
 * @param desc the description we wish to use
 * @param ptr Function pointer to the malloc init function to use*/
void Description_setMyMallocInit(SDescription *desc, personalized_malloc_init_t ptr);

/**
 * @brief Sets the pointer for the allocation function of our allocation library
 * @param desc the description we wish to use
 * @param ptr Function pointer to the malloc function to use*/
void Description_setMyMalloc(SDescription *desc, personalized_malloc_t ptr);

/**
 * @brief Sets the pointer for the free function of our allocation library
 * @param desc the description we wish to use
 * @param ptr Function pointer to the free function to use*/
void Description_setMyFree(SDescription *desc, personalized_free_t ptr);

/**
 * @brief Sets the pointer for the destructor function of our allocation library
 * @param desc the description we wish to use
 * @param ptr Function pointer to the destructor function to use*/
void Description_setMyMallocDestroy(SDescription *desc, personalized_malloc_destroy_t ptr);

/**
 * @brief Gets the dynamic allocation library's path
 * @param desc the description we wish to use
 * @return The path of the allocation library*/
char* Description_getDynAllocLib(SDescription *desc);

/**
 * @brief Gets the pointer for the initialization function of our allocation library
 * @param desc the description we wish to use
 * @return The pointer for the initialization function of our allocation library*/
personalized_malloc_init_t Description_getMyMallocInit(SDescription *desc);

/**
 * @brief Gets the pointer for the allocation function of our allocation library
 * @param desc the description we wish to use
 * @return The pointer for the allocation function of our allocation library*/
personalized_malloc_t Description_getMyMalloc(SDescription *desc);

/**
 * @brief Gets the pointer for the free function of our allocation library
 * @param desc the description we wish to use
 * @return The pointer for the free function of our allocation library*/
personalized_free_t Description_getMyFree(SDescription *desc);

/**
 * @brief Gets the pointer for the destructor function of our allocation library
 * @param desc the description we wish to use
 * @return The pointer for the destructor function of our allocation library*/
personalized_malloc_destroy_t Description_getMyMallocDestroy(SDescription *desc);

/**
 * @brief Gets the resuming status 
 * @param desc the description we wish to use
 * @return Returns 0 if the current run is fresh, 1 otherwise*/
int Description_getResuming(SDescription *desc);

/**
 * @brief Gets the number of resumes until now
 * @param desc the description we wish to use
 * @return Returns the number of resumes until now */
int Description_getNumber_of_resumes(SDescription *desc);

/**
 * @brief Sets the resuming status
 * @param desc the description we wish to use
 * @param val 1 if we're resuming, 0 if not*/
void Description_setResuming(SDescription *desc, int val);

/**
 * @brief Sets the number of resumes
 * @param desc the description we wish to use
 * @param val The number of resumes*/
void Description_setNumber_of_resumes(SDescription *desc, int val);

/**
 * @brief Gets the resume id
 * @param desc the description we wish to use
 * @return Returns the resume id */
int Description_getResumeId(SDescription *desc);

/**
 * @brief Sets the resume id
 * @param desc the description we wish to use
 * @param val The new resume id*/
void Description_setResumeId(SDescription *desc, int val);

/**
 * @brief Sets whether or not the input executable can show anything in the terminal or not
 * @param desc the description we wish to use
 * @param val the value we wish to set
 */
void Description_setSuppressOutput (SDescription *desc, int val);

/**
 * @brief Gets whether or not the input executable can show anything in stdout or not
 * @param desc the description we wish to use
 * @return Returns whether or not the input executable can show anything in the terminal or not
 */
int Description_getSuppressOutput (SDescription *desc);

/**
 * @brief Sets the filestream where the executable output has to print
 * @param desc the description we wish to use
 * @param file the filestream we wish to use
 */
void Description_setOutputFileStream (SDescription *desc, char *value);

/**
 * @brief Gets the filestream where the executable output has to print
 * @param desc the description we wish to use
 * @return the filestream where the executable output has to print
 */
char *Description_getOutputFileStream (SDescription *desc);

/**
 * @brief Sets the kernel function name for initializing benchmark arrays
 * @param desc the description we wish to use
 * @param value the string refering to the kernel function name
 */
void Description_setKernelInitFunctionName (SDescription *desc, char *value);

/**
 * @brief Gets the kernel function name for initializing benchmark arrays
 * @param desc the description we wish to use
 * @return the string refering to the kernel function name
 */
char *Description_getKernelInitFunctionName (SDescription *desc);

/**
 * @brief Sets the kernel function for initializing benchmark arrays
 * @param desc the description we wish to use
 * @param value the initialization kernel function
 */
void Description_setKernelInitFunction (SDescription *desc, void *func);

/**
 * @brief Gets the kernel function for initializing benchmark arrays
 * @param desc the description we wish to use
 * @return the initialization kernel function
 */
void *Description_getKernelInitFunction (SDescription *desc);

/**
 * @brief Sets the vector elements size in bytes
 * @param desc the description we wish to use
 * @param value the value we wish to set
 */
void Description_setVectorElementSize (SDescription *desc, int value);

/**
 * @brief Gets the vector elements size in bytes
 * @param desc the description we wish to use
 * @return the vector elements size in bytes
 */
int Description_getVectorElementSize (SDescription *desc);

/**
 * @brief Sets a vector size at a specific offset
 * @param desc the description we wish to use
 * @param value the value we wish to set
 * @param idX the id of the vector we want the size of
 */
void Description_setVectorSizeAt (SDescription *desc, int value, int idX);

/**
 * @brief Sets the pinning for each process
 * @param desc the description we wish to use
 * @param value the value we wish to set
 * @param idX the id of the vector we want the size of
 */
void Description_setPinningAt (SDescription *desc, int value, int idX);

void Description_setVectorSizes (SDescription *desc, int value);

/**
 * @brief Gets a vector size
 * @param desc the description we wish to use
 * @param idX the id of the vector we want the size of
 * @return the requested vector size
 */
int Description_getVectorSizeAt (SDescription *desc, int idX);

/**
 * @brief Defines whether or not the nbSizes parameter is defined
 * @param desc the description we wish to use
 * @return whether or not the nbSizes parameter is defined
 */
int Description_isNbSizeDefined (SDescription *desc);

/**
 * @brief Tell the description we're using different sizes of vectors
 * @param desc the description we wish to use
 */
void Description_nbSizesEnable (SDescription *desc);

/**
 * @brief Tell the description we're not using different sizes of vectors
 * @param desc the description we wish to use
 */
void Description_nbSizesDisable (SDescription *desc);

/**
 * @brief Set the data returned by the evaluation library
 * @param desc the description we wish to use
 * @param data the content we wish to keep track of
 */
void Description_setEvaluationData (SDescription *desc, void *data, unsigned idX);

/**
 * @brief Get the evaluation library data
 * @param desc the description we wish to use
 * @return the data from the evaluation library
 */
void *Description_getEvaluationData (SDescription *desc, unsigned idX);

/**
 * @brief Enables the thread pinning in Executable mode
 * @param desc the description we wish to use
 */
void Description_pinThreadEnable (SDescription *desc);

/**
 * @brief Disables the thread pinning in Executable mode
 * @param desc the description we wish to use
 */
void Description_pinThreadDisable (SDescription *desc);

/**
 * @brief Check if the thread pinning is enabled or not
 * @param desc the description we wish to use
 * @return Whether or not the thread pinning is enabled
 */
int Description_isThreadPinningEnabled (SDescription *desc);

/**
 * @brief Enables the "all processes are going to make an output file" mode
 * @param desc the description we wish to use
 */
void Description_allProcessOutputEnable (SDescription *desc);

/**
 * @brief Disables the "all processes are going to make an output file" mode
 * @param desc the description we wish to use
 */
void Description_allProcessOutputDisable (SDescription *desc);

/**
 * @brief Check if the "all processes are going to make an output file" mode is selected or not
 * @param desc the description we wish to use
 * @return Whether or not the "all processes are going to make an output file" mode is selected
 */
int Description_isAllProcessOutputEnabled (SDescription *desc);

/**
 * @brief The current process now has to handle printing stuff
 * @param desc the description we wish to use
 */
void Description_printingProcessEnable (SDescription *desc);

/**
 * @brief The current process now must not handle printing stuff
 * @param desc the description we wish to use
 */
void Description_printingProcessDisable (SDescription *desc);

/**
 * @brief Check if the current process has to handle printing stuff
 * @param desc the description we wish to use
 * @return Whether or not the current process has to handle printing stuff
 */
int Description_isPrintingProcess (SDescription *desc);

/**
 * @brief The current process now has to handle evaluation library stuff
 * @param desc the description we wish to use
 */
void Description_processEvalHandlerEnable (SDescription *desc);

/**
 * @brief The current process now must not handle evaluation library stuff
 * @param desc the description we wish to use
 */
void Description_processEvalHandlerDisable (SDescription *desc);

/**
 * @brief Check if the current process now has to handle evaluation library stuff or not
 * @param desc the description we wish to use
 * @return Whether or not the current process now has to handle evaluation library stuff
 */
int Description_isProcessEvalHandler (SDescription *desc);

/**
 * @brief Sets the process Id of the current process
 * @param desc the description we wish to use
 * @param value the process Id of the current process
 */
void Description_setProcessId (SDescription *desc, int value);

/**
 * @brief Gets the process Id of the current process
 * @param desc the description we wish to use
 * @return the process Id of the current process
 */
int Description_getProcessId (SDescription *desc);

/**
 * @brief Enables the creation of the XLS summary file
 * @param desc the SDescription we wish to use
 */
void Description_enableSummary (SDescription *desc);

/**
 * @brief Disables the creation of the XLS summary file
 * @param desc the SDescription we wish to use
 */
void Description_disableSummary (SDescription *desc);

/**
 * @brief Whether or not the creation of the XLS summary file is enabled
 * @param desc the SDescription we wish to use
 * @return Whether or not the creation of the XLS summary file is enabled
 */
int Description_isSummaryEnabled (SDescription *desc);

/**
 * @brief Tells the program OK I have to pin my program on a CPU
 * @param desc the SDescription we wish to use
 */
void Description_defineCpuPin (SDescription *desc);

/**
 * @brief Tells the program OK I don't have to pin my program on a CPU
 * @param desc the SDescription we wish to use
 */
void Description_undefineCpuPin (SDescription *desc);

/**
 * @brief Tells whether or not the program has to be pinned on a CPU
 * @param desc the SDescription we wish to use
 * @return whether or not the program has to be pinned on a CPU
 */
int Description_isCpuPinDefined (SDescription *desc);

/**
 * @brief Sets the verification library name
 * @param desc the SDescription we wish to use
 * @param value The value to be set
 */
void Description_setVerificationLibraryName (SDescription *desc, char *value);

/**
 * @brief Gets the verification library name
 * @param desc the SDescription we wish to use
 * @param return The verification library name
 */
char *Description_getVerificationLibraryName (SDescription *desc);

/**
 * @brief Sets the verification context data (typically a structure containing data from the library)
 * @param desc the SDescription we wish to use
 * @param data The data to be set
 */
void Description_setVerificationContextData (SDescription *desc, void *data);

/**
 * @brief Gets the verification context data
 * @param desc the SDescription we wish to use
 * @return the verification context data
 */
void *Description_getVerificationContextData (SDescription *desc);

/**
 * @brief Sets the verification init function pointer
 * @param desc the SDescription we wish to use
 * @param data the verification init function pointer we want to be set
 */
void Description_setVerificationInitFct (SDescription *desc, verificationFctInit fct);

/**
 * @brief Gets the verification init function pointer
 * @param desc the SDescription we wish to use
 * @return the verification init function pointer
 */
verificationFctInit Description_getVerificationInitFct (SDescription *desc);

/**
 * @brief Sets the verification display function pointer
 * @param desc the SDescription we wish to use
 * @param data the verification display function pointer we want to be set
 */
void Description_setVerificationDisplayFct (SDescription *desc, verificationFctDisplay fct);

/**
 * @brief Gets the verification display function pointer
 * @param desc the SDescription we wish to use
 * @return the verification display function pointer
 */
verificationFctDisplay Description_getVerificationDisplayFct (SDescription *desc);

/**
 * @brief Sets the verification destroy function pointer
 * @param desc the SDescription we wish to use
 * @param data the verification destroy function pointer we want to be set
 */
void Description_setVerificationDestroyFct (SDescription *desc, verificationFctDestroy fct);

/**
 * @brief Gets the verification destroy function pointer
 * @param desc the SDescription we wish to use
 * @return the verification destroy function pointer
 */
verificationFctDestroy Description_getVerificationDestroyFct (SDescription *desc);

/**
 * @brief Sets the number of filenames to be launched by the program
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_setKernelFileNamesTabSize (SDescription *desc, unsigned value);

/**
 * @brief Gets the number of filenames to be launched by the program
 * @param desc the SDescription we wish to use
 * @return the number of filenames to be launched by the program
 */
unsigned Description_getKernelFileNamesTabSize (SDescription *desc);

/**
 * @brief Sets the kernelfilename to be executed by the program
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 * @param idX The id of the current filename to be set
 */
void Description_setKernelFileNameAt (SDescription *desc, char *value, unsigned idX);

/**
 * @brief Changes the current file ID of the description
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_setKernelCurrentFileId (SDescription *desc, unsigned value);

/**
 * @brief Gets the current file ID of the description
 * @param desc the SDescription we wish to use
 * @return the current file ID of the description
 */
unsigned Description_getKernelCurrentFileId (SDescription *desc);

/**
 * @brief Sets the number of evaluation librairies to be used
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_setNbEvaluationLibrairies (SDescription *desc, unsigned value);

/**
 * @brief Gets the number of evaluation librairies to be used
 * @param desc the SDescription we wish to use
 * @return the number of evaluation librairies to be used
 */
unsigned Description_getNbEvaluationLibrairies (SDescription *desc);

/**
 * @brief Parses the evaluation library input and set data into the description
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_parseEvaluationLibraryInput (SDescription *desc, char *value);

/**
 * @brief Parses the info argument input and set data into the description
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_parseInfoDisplayedInput (SDescription *desc, char *value);

/**
 * @brief Enables the eval stack mode
 * @param desc the SDescription we wish to use
 */
void Description_enableEvalStack (SDescription *desc);

/**
 * @brief Disables the eval stack mode
 * @param desc the SDescription we wish to use
 */
void Description_disableEvalStack (SDescription *desc);

/**
 * @brief Check whether or not the eval stack mode is enable
 * @param desc the SDescription we wish to use
 * @return whether or not the eval stack mode is enable
 */
int Description_isEvalStackEnabled (SDescription *desc);

void Description_setOutputFileName (SDescription *desc, char *value);
char *Description_getOutputFileName (SDescription *desc);

/**
 * @brief Changes execution path
 * @param desc the SDescription we wish to use
 * @param value The value we wish to set
 */
void Description_setExecPath (SDescription *desc, char *value);

/**
 * @brief Returns the execution path
 * @param desc the SDescription we wish to use
 * @return the execution path
 */
char *Description_getExecPath (SDescription *desc);

/**
 * @brief Reformat strings using the process information
 * @param desc the SDescription we wish to use
 * @param procId the process Id
 */
void Description_reformatStrings (SDescription *desc, int procId);

/**
 * @brief Set whether all sons print out
 * @param v the value
 */
void Description_setAllPrintOut (SDescription *desc, int v);

/**
 * @brief Get whether all sons print out
 * @return the value
 */
int Description_getAllPrintOut (SDescription *desc);
#endif
