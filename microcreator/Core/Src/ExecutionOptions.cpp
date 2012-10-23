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
 @file ExecutionOptions.cpp
 @brief The ExecutionOptions class is in this file
 */

#include <cassert>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <list>

#include "Driver.h"
#include "ExecutionOptions.h"
#include "Logging.h"
#include "PassEngine.h"
#include "PluginPass.h"

ExecutionOptions::ExecutionOptions (int ac, char **av, Description *desc, Driver *driver)
{

	static int verbose_flag; //Flag set by `--verbose`
	pluginPath = "";
	pluginCmd = false;
	passRef = "";
	pluginPos = PASS_POS_UNKNOWN;
	handlingOkay = true;

	static option long_options[] =
		{
			//These options set a flag
			{ "verbose", no_argument, &verbose_flag, 1 },
			{ "brief", no_argument, &verbose_flag, 0 },

			//These options don't set a flag. We distinguish them by their indices
			{ "asm", no_argument, 0, 'c' },			//Assembly inline
			{ "list-passes", no_argument, 0, 'l' },	//List of passes currently used
			{ "file-number", required_argument, 0, 'n' },	//Number of file to proceed
			{ "help", no_argument, 0, 'h' },				//Help
			{ "version", no_argument, 0, 'v' },	//Version of the tool
			{ "fplugin", required_argument, 0, 'f' }, 	//Plugin path
			{ 0, 0, 0, 0 }
		};

	//getopt_long stores the option index here
	int option_index = 0;
	int opt;

	//Detect the end of the options
	while ((opt = getopt_long (ac, av, "cln:hvf:", long_options, &option_index)) != -1)
	{
		switch (opt)
		{
			case 0:
				//If this option set a flag, do nothing else now
				if (long_options[option_index].flag != 0)
				{
					break;
				}

				std::cout << "option " << long_options[option_index].name;

				if (optarg)
				{
					std::cout << " with arg " << optarg << std::endl;
				}
				break;

			case 'c':
				{
					if (desc != NULL)
					{
						//Set the c extension to the output
						desc->setOutputExtension (".c");

						//Inlinig the assembly code
						desc->setAsmVolatile (true);
					}
				}
				break;

			case 'l':
				{
					if (driver != NULL)
					{
						PassEngine *currentPassEngine = driver->getPassEngine ();
						std::list<Pass*> currentListPasses;
						currentListPasses = currentPassEngine->getListPasses ();

						//There no passes if size is 0
						if (currentListPasses.size () == 0)
						{
							Logging::log (1, "Warning: PassEngine has no passes, it is stopping", NULL);
							return;
						}

						std::cout << "The list of passes currenty used by the driver: " << std::endl;

						//Print the list of passes currently used
						for (std::list<Pass *>::iterator it = currentListPasses.begin (); it != currentListPasses.end (); it++)
						{
							Pass *p = *it;
							std::cerr << "-> " << p->getName () << std::endl;
						}
					}
				}
				break;

			case 'h':
				{
					differentOptions ();
				}
				break;

			case 'v':
				{
					std::cout << "\033[1mMicro-creator version: 1.0\033[0m" << std::endl;
				}
				break;

			case 'n':
				{
					std::istringstream iss (optarg);
					int startCounter;

					//Register it
					iss >> startCounter;

					desc->setFileCounterStart (startCounter);
				}
				break;

			case 'f': //Plugin execution option
				{
					//Get the name of the plugin and the path to this plugin
					std::string pluginStr = "";

					if (getPluginName (optarg, pluginStr) == false)
					{
						return;
					}

					//Open the shared library
					void* plugin = dlopen (pluginStr.c_str (), RTLD_LAZY);

					if (!plugin)
					{
						std::cerr << "Cannot load library : " << dlerror () << std::endl;
						return;
					}

					void (*pluginInit) (Driver*, Description*);
					pluginInit = (void(*) (Driver*, Description*)) dlsym (plugin, "pluginInit");

					//Paranoid
const					char *dlsym_error = dlerror ();

					if (dlsym_error)
					{
						std::cerr << "Cannot load symbol initPlugin: " << dlsym_error << std::endl;
						return;
					}

					if ((driver != NULL) && (desc != NULL))
					{
						pluginInit (driver, desc);
					}
				}
				break;

			default: // Do nothing
				break;
		}

		//We report the final status resulting from `--verbose` and `--brief`
		if (desc != NULL)
		{
			if (verbose_flag == 1)
			{
				desc->setVerbose (true);
			}
			else
			{
				desc->setVerbose (false);
			}
		}
	}
}

ExecutionOptions::~ExecutionOptions (void)
{

}

bool ExecutionOptions::getHandlingOkay (void) const
{
	return handlingOkay;
}

void ExecutionOptions::setHandlingOkay (bool val)
{
	handlingOkay = val;
}

PluginType ExecutionOptions::getPluginFileType (const std::string &optarg, std::string &folder, std::string &file)
{
	PluginType src = UNKNOWN_FILE;

	unsigned int idx = optarg.find_last_of ("/");

	folder = optarg.substr (0, idx);
	file = optarg.substr (idx + 1);

	if (file != "")
	{
		unsigned int idx_type = file.find_last_of (".");
		std::string type = "";
		type = file.substr (idx_type);

		if (type == ".so")
		{
			src = LIBRARY_FILE;
		}
		else if ((type == ".c") || (type == ".cpp"))
		{
			src = SOURCE_FILE;
		}
	}

	return src;
}

bool ExecutionOptions::getPluginName (const std::string optarg, std::string &pluginStr)
{
	//Get the file type: is it a library or a cpp file?
	PluginType pluginType = UNKNOWN_FILE;
	std::string folder = "", file = "";

	//Get the type of plugin 
	pluginType = getPluginFileType (optarg, folder, file);

	std::ostringstream oss;

	if (pluginType == SOURCE_FILE)
	{
		std::string name = file.substr (0, file.find_last_of ("."));

		oss << folder << "/" << name << ".so";
		pluginStr = oss.str ().c_str ();

		oss.str ("");

		oss << "g++ " << folder << "/" << file << " -o " << pluginStr << " -fPIC -shared -ICore/Include";

		unsigned int res = system (oss.str ().c_str ());

		//If call system failed (compilation failed)
		if (res != 0)
		{
			std::cout << "Compilation of file `" << folder << "/" << file << "` failed, MicroCreator will exit now" << std::endl;
			setHandlingOkay (false);
			return false;
		}
	}
	else if (pluginType == LIBRARY_FILE)
	{
		pluginStr = optarg;
	}

	return true;
}

void ExecutionOptions::differentOptions (void)
{
	std::cout << "\033[1mNAME\033[0m" << std::endl;
	std::cout << "\tMicroCreator - micro-benchmarks generator \n" << std::endl;
	std::cout << "\033[1mSYNOPSIS\033[0m" << std::endl;
	std::cout << "\t./microcreator   [DESCRIPTION_PATH] [--version] [--help] [--verbose] \n" << std::endl;
	std::cout << "\t\t\t [--brief] [--asm] [--list-passes] [--fplugin=PLUGIN_PATH] \n" << std::endl;
	std::cout << "\033[1mDESCRIPTION\033[0m" << std::endl;
	std::cout << "\tMicroCreator is a benchmark programs creator. It is used to create programs with slight differences in order to analyze\n" << std::endl;
	std::cout << "\tthese slight changes' impact on the underlying architecture. It is a C++ project and a stand-alone program.\n" << std::endl;
	std::cout << "\033[1mOPTIONS\033[0m" << std::endl;
	std::cout << "\t--version, -v" << std::endl;
	std::cout << "\t\tPrints the MicroCreator version.\n" << std::endl;
	std::cout << "\t--help, -h" << std::endl;
	std::cout << "\t\tPrints the synopsis and a list of the different options.\n" << std::endl;
	std::cout << "\t--verbose, -v" << std::endl;
	std::cout << "\t\tSets the system into a verbosis tool. It creates an output file for each pass, giving extra information on what has happenned.\n" << std::endl;
	std::cout << "\t--brief, -b" << std::endl;
	std::cout << "\t\tSets the system into a silent mode. It disables the verbosis made.\n" << std::endl;
	std::cout << "\t--asm, -c" << std::endl;
	std::cout << "\t\tGenerates C code using asm instructions in the kernel.\n" << std::endl;
	std::cout << "\t--list-passes, -l" << std::endl;
	std::cout << "\t\tDisplays the list of passes currently used.\n" << std::endl;
	std::cout << "\t--fplugin=<path/to/plugin.so>" << std::endl;
	std::cout << "\t\tCall a plugin.\n" << std::endl;
	std::cout << "\t--n <number> " << std::endl;
	std::cout << "\t\tSet the start file number counter.\n" << std::endl;
	std::cout << "\033[1mDOCUMENTATION\033[0m" << std::endl;
	std::cout << "\tThe documentation for the MicroCreator tool is available on the Exascale internal wiki.\n" << std::endl;
}
