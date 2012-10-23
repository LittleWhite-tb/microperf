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
  @file PluginChange.cpp
  @brief The PluginChange plugin is in this file 
 */

#include <iostream>
#include <cassert>
#include <sstream>

#include "Driver.h"
#include "Instruction.h"
#include "Kernel.h"
#include "Logging.h"
#include "MemoryOperand.h"
#include "PassElement.h"

static const int LOAD_OFFSET_INCREMENT = 8;
static const int LOAD_START_OFFSET = 8;
static const int STORE_OFFSET = 0;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//   This plugin pass keep only the kernels with the same unrolling factor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static bool chooseKernel (Kernel *kernel);
static bool handleKernel (Kernel *kernel);
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

	if (handleKernel (kernel) == true)
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
static bool handleKernel (Kernel *kernel)
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		//Now handle this Kernel
		if (chooseKernel (kernel) == false)
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
            if (handleKernel (inner) == false)
                return false;
		}
	}

    return true;
}

static bool chooseKernel (Kernel *kernel) 
{
	if (kernel == NULL)
	{
		return false;	
	}

	std::string str = "";

    //Start at 8 because store is first
    int offset = LOAD_START_OFFSET;

	unsigned int nbr = kernel->getNbrStatements (), i;

    unsigned int cnt = 0, outOf = 0;

    //First check if we have only one store
	for (i = 0; i < nbr; i++)
	{
		//Get statement
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != 0);

		//We only care about instructions
		Instruction *inst = dynamic_cast<Instruction *> (stmt);	 

		if (inst != 0)
		{
            //Ok, now see if it's a store
            Operand *op = inst->getModifiableOperand (0);
            
            MemoryOperand *memop = dynamic_cast<MemoryOperand *> (op);

            //It's a store
            if (memop == 0)
            {
                cnt++;
            }

            outOf++;
        }
    }

    if (cnt != 1 && outOf == 8)
        return false;
    
    //Now we can go through it and correct it
	for (i = 0; i < nbr; i++)
    {
		//Get statement
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != 0);

		//We only care about instructions
		Instruction *inst = dynamic_cast<Instruction *> (stmt);	 

		if (inst != 0)
		{
            //Ok, now see if it's a load
            Operand *op = inst->getModifiableOperand (0);
            
            MemoryOperand *memop = dynamic_cast<MemoryOperand *> (op);

            if (memop != 0)
            {
                //It is a load
                memop->setOffset (offset);
                offset += LOAD_OFFSET_INCREMENT;
            }
            else
            {
                //It is a store, set the offset to 0
                memop = dynamic_cast<MemoryOperand*> (inst->getModifiableOperand (1));

                assert (memop != 0);

                memop->setOffset (STORE_OFFSET);
            }
		}
	}

    return true;
}

extern "C" void pluginInit (Driver *driver, const Description *desc)
{
	//PluginPass
	SPluginPassInfo plugInfo1 = {myPluginName, myPluginGate, myPluginEntry, "Operand swap after unroll", PASS_POS_INSERT_AFTER};
	PluginPass *pass1 = new PluginPass (plugInfo1);
	pass1->pluginAddPass (driver, plugInfo1);

    (void) desc;
}

