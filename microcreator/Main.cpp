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
  @mainpage MicroCreator is a microbenchmark generator
  @version 1.00
  @author Exascale Research Center
  @date 01/03/2012
 */

/**
  @file Main.cpp
  @brief The Main function is in this file 
 */

#include <iostream>
#include <stdlib.h>

#include "DescriptionXML.h"
#include "Driver.h"
#include "ExecutionOptions.h"
#include "ImmediateSelection.h"
#include "Logging.h"

int main (int argc, char **argv)
{
	int res = 0;
	const char *version = "1.02";
	std::string fileName = "";

	Logging::log (0, "Micro-creator version: ", version, NULL);

	//Delete generated benchmarks previously
    res = system ("rm -f output/*");

    if (res != 0)
    {
        Logging::log (0, "Problem with deleting previous benchmarks in output directory", NULL);
        //Close logging
        Logging::shutDown ();
        return EXIT_FAILURE;
    }

	if (argc == 1)
	{
		Logging::log (0, "No arguments, will print out usage and exit", NULL);

        //Close logging
        Logging::shutDown ();
		std::cout << "Usage: " << argv[0] << " <description file>" << std::endl;
		return EXIT_SUCCESS;
	}

	//Handle the command-line options
	ExecutionOptions *options = NULL;

	//Create the Driver
	Driver *driver = new Driver ();

	//Add the general passes
	driver->addGeneralPasses ();

	if (argc == 2)
	{
		//We get the first parameter (name of the input file)
		fileName = argv[1];

		//If we have an input file and not an option (beginning with -)
		if (fileName[0] == '-')
		{
            //Reorder them
			options = new ExecutionOptions (argc, argv, NULL, driver);

			//Free it 
			delete options, options = NULL;

            //Close logging
            Logging::shutDown ();
			return EXIT_SUCCESS;
		}
	}

	//Get the file name from the list of parameters because it can be the first parameter or the last
	for (int i = 1; i < argc; i++)
	{
		std::string str = argv[i];

		if (str[0] != '-')
		{
			fileName = str;
			break;
		}
	}

	//Free it
	delete driver, driver = NULL;

	//Create the Description
	DescriptionXML *description = new DescriptionXML (fileName);

	if (description->getParsingIsOkay () == false)
	{
		Logging::log (2, "Error: XML parsing error, program will exit", NULL);
		std::cerr << "There is a problem with the xml input file, check the log file 'Log.txt' for more details" << std::endl;
        delete description, description = NULL;
        //Close logging
        Logging::shutDown ();
		return EXIT_FAILURE;
	}

	//Create the Driver with the new description
	driver = new Driver (description);

	//Add the general passes
	driver->addGeneralPasses ();

	options = new ExecutionOptions (argc, argv, description, driver);

	//If the handling of the command-system is done without problem
	if (options->getHandlingOkay () == false)
	{
		//Free it
		delete options, options = NULL;

        //Close logging
        Logging::shutDown ();
		return EXIT_FAILURE;
	}

	//Free it
	delete options, options = NULL;

	//Now launch it
	driver->drive ();

	Logging::log (0, "Micro-creator shutting down", NULL);

	//Close logging
	Logging::shutDown ();

	//Free it
	delete driver, driver = NULL;

	return EXIT_SUCCESS;
}
