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
  @file PluginFilterPass.cpp
  @brief The PluginFilterPass plugin is in this file 
 */

#include <iostream>
#include <cassert>
#include <sstream>

#include "Comment.h"
#include "Driver.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//   This plugin pass keep only the kernels with the same unrolling factor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static bool chooseKernel (Kernel *kernel, Description *desc, int &unrollFactor);
static bool handleKernel (Kernel *kernel, Description *desc, int &unrollFactor);
static int extractInt (const std::string &str);
//-----------------------------------------------------------------------------
//-------------------------------Local variables-------------------------------
//-----------------------------------------------------------------------------

static const std::string& ourName = "Filtering Plugin";		/** < @brief The plugin name */
static bool ourGate = true; 								/** < @brief The plugin gate */

//-----------------------------------------------------------------------------
//-----------------------------Functions definition----------------------------
//-----------------------------------------------------------------------------

extern "C" bool myPluginGate (const Description *desc, const Kernel *kernel)
{
    (void) desc;
    (void) kernel;
	return ourGate;
}

extern "C" const std::string& myPluginName (void)
{
	return ourName;
}

std::vector <PassElement *> *myPluginEntry (PassElement *pe, Description *desc)  
{
	Logging::log (0, "Starting Filtering", NULL);

	//First get Kernel
	Kernel *kernel = pe->getKernel ();

	if (kernel == NULL)
	{
		return NULL;
	}

	//Clear color
	kernel->clearColor ();    

	//Prepare result
	std::vector <PassElement *> *res = NULL;

	//Create new vector for kernels
	std::vector<Kernel*> newKernels;

	//Now we start filtering
	int unrollFactor = -1;
	bool keepIt = handleKernel (kernel, desc, unrollFactor);

	if (keepIt == true)
	{
        //Create new vector
        res = new std::vector <PassElement *> ();
        //Ok we want this one, push it in
        res->push_back (pe);
	}

	Logging::log (0, "Stopping Filtering", NULL);

	(void) desc;	

    //Return result
	return res;
}

//-----------------------------------------------------------------------------
//-------------------------- Particular pass functions ------------------------
//-----------------------------------------------------------------------------
static bool handleKernel (Kernel *kernel, Description *desc, int &unrollFactor)
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		//Now handle this Kernel: if over, no need to continue
		if (chooseKernel (kernel, desc, unrollFactor) == false)
			return false;
	}

	//Now go into the kernel
	unsigned int nbr = kernel->getNbrStatements ();

	//Now go into this Kernel and find the inner kernels
	for (unsigned int i = 0; i < nbr; i++)
	{
		//Get the instruction
		Statement *stmt = kernel->getModifiableStatement (i);

		//If it's a Kernel, go into it
		Kernel *inner = dynamic_cast<Kernel *> (stmt);

		if (inner != NULL)
		{
			bool tmp = handleKernel (inner, desc, unrollFactor);

			//Only stop if tmp is false
			if (tmp == false)
			{
				return false;
			}
		}
	}

	return true;
}

static bool chooseKernel (Kernel *kernel, Description *desc, int &unrollFactor) 
{
	if (kernel == NULL)
	{
		return true;	
	}

	std::string str = "";

	unsigned int nbr = kernel->getNbrStatements ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		//Get statement
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//We only care about comment
		Comment *comment = dynamic_cast<Comment *> (stmt);	 

		if (comment != NULL)   
		{
			stmt->getString (str, desc, 0);

			//We care about '#Unroll factor'
			if (str.find ("#Unrolled factor") != str.npos)
			{
				//The first unroll comment update the factor of unrolling
				if (unrollFactor == -1)  
				{
					//Update the unroll factor
					unrollFactor = extractInt (str);
				}
				else
				{
					if (extractInt (str) != unrollFactor)
					{	
						return false;
					}
				}			
			}
		}
	}

	return true;
}

static int extractInt (const std::string &str)
{
	int idx = str.find_last_of (' ');

	//If not found
	if (idx < 0)
		return -1;

	//Go after factor
	idx++;

	const std::string st = str.substr (idx);
	std::stringstream iss(st);
	unsigned int number;
	iss >> number;
	return number;
}

extern "C" void pluginInit (Driver *driver, const Description *desc)
{
	//PluginPass
	SPluginPassInfo plugInfo1 = {myPluginName, myPluginGate, myPluginEntry, "Pass unroll", PASS_POS_INSERT_AFTER};
	PluginPass *pass1 = new PluginPass (plugInfo1);
	pass1->pluginAddPass (driver, plugInfo1);

    (void) desc;
}

