/**
@mainpage Micro-detector creates one of the configuration files used by the micro-launcher and micro-generator
@version 1.00
@author Jean Christophe Beyler
@date 04/11/2010
*/

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "Driver.h"
#include "Logging.h"

/**
 * @brief Initial check
 * @param source source filename
 * @param dest destination filename
 * @return EXIT_SUCCESS, EXIT_FAILURE
 */
static int initial_check (char *source, char *dest);

/* This program detects the registers used by the system when passing parameters
 *  - This will be used by the two other projects to know what registers are used when calling micro-benchmark programs
 */

int
main (int argc, char **argv)
{
    Logging::log (0, "Starting micro-detector", NULL);
    
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <source for detection> <destination file>" << std::endl;
        return EXIT_SUCCESS;
    }

    //Initial check
    if (initial_check (argv[1], argv[2]) == EXIT_FAILURE)
        return EXIT_FAILURE;

    //Ok now we can work
    Driver driver (argv[1], argv[2]);
    if (driver.drive () == false)
    {
        Logging::log (0, "Driver failed", NULL);
        return EXIT_FAILURE;
    }

    Logging::log (0, "Ending micro-detector", NULL);
    return EXIT_SUCCESS;
}

int
initial_check (char *source, char *dest)
{
    Logging::log (0, "Opening source file", NULL);

    //Open source
    std::ifstream in (source, std::ios::in);

    if (in.is_open () == false)
    {
        Logging::log (0, "Init: Failed to open source: ", source, NULL);
        return EXIT_FAILURE;
    }

    //Close it, we just wanted to check
    in.close ();

    //Open destination
    std::ofstream out (dest, std::ios::out);

    if (out.is_open () == false)
    {
        Logging::log (0, "Init: Failed to open destination: ", dest, NULL);
        return EXIT_FAILURE;
    }

    //Close it, we just wanted to check
    out.close ();

    //All good
    return EXIT_SUCCESS;
}
