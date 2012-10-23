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
  @file OperandSwap.cpp
  @brief The OperandSwap pass is in this file 
 */

#include <cassert>
#include <fstream>

#include <iostream>

#include "Description.h"
#include "OperandSwap.h"
#include "Instruction.h"
#include "PassElement.h"
#include "Kernel.h"
#include "Logging.h"

OperandSwap::OperandSwap (bool bu)
{
	beforeUnroll = bu;

    //Set name depending on bu
	if (bu)
	{
		name = "Operand swap before unroll";
	}
	else
	{
		name = "Operand swap after unroll";
	}
}

OperandSwap::~OperandSwap (void)
{
}

std::vector <PassElement *> *OperandSwap::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Operand Swap", NULL);

	//First get Kernel
	Kernel *kernel = pe->getKernel ();

	if (kernel == NULL)
		return NULL;

	//Clear color
	kernel->clearColor ();

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//Create new vector for kernels
	std::vector<Kernel*> newKernels;

	//Push original kernel here
	newKernels.push_back (kernel);

	//Handle the Kernels
	handlePass (newKernels);

	//Now accept these all new kernels
	for (std::vector <Kernel *>::const_iterator it_kernel = newKernels.begin (); it_kernel != newKernels.end (); it_kernel++)
	{
		Kernel *tmp = *it_kernel;
		PassElement *pe_tmp = NULL;

		//If we have our original kernel, we can use pe
		if (kernel == tmp)
		{
			pe_tmp = pe;
		}
		else
		{
            //Create new PassElement
			pe_tmp = new PassElement ();
		}

        //Set new kernel and push to result vector
		pe_tmp->setKernel (tmp);
		res->push_back (pe_tmp);
	}

	Logging::log (0, "Stopping Operand Swap", NULL);

	(void) desc;

	return res;
}        

void OperandSwap::handlePass (std::vector<Kernel *> &kernels) const
{
	unsigned int i = 0;

	//Careful we do this on purpose: kernels can change as we are going through this
	while (i < kernels.size ())
	{
		Kernel *kernel = kernels[i];

		//Now we handle this Kernel and generate all the Stride variations we want
		handleKernel (kernels, kernel, kernel);

		//Increment i
		i++;
	}
}

void OperandSwap::handleKernel (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel) const
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		//Now handle this Kernel
		handleSwap (kernels, outer, kernel);
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
			handleKernel (kernels, outer, inner);
		}
	}
}

void OperandSwap::handleSwap (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel, unsigned int start) const
{	
	unsigned int nbr = kernel->getNbrStatements ();

	for (unsigned int i = start; i < nbr; i++)
	{
		//Get instruction
		Statement *s_interest = kernel->getModifiableStatement (i);

		//Paranoid
		assert (s_interest != NULL);

		//We only care about instructions
		const Instruction *interest = dynamic_cast<const Instruction *> (s_interest);

		if (interest != NULL)
		{
			//Ok now do we swap this instruction ? This depends on the member beforeUnroll
			bool test = (beforeUnroll ? interest->getSwapBefore () : interest->getSwapAfter ());

			if (test == true)
			{
				//Ok then we create a new PassElement											

				//Create a new kernel
				Kernel *outer_copy = dynamic_cast<Kernel*> (outer->copy ());

				//Paranoid
				assert (outer_copy != NULL);

				//Now find our Kernel
				Kernel *copy = dynamic_cast<Kernel*> (outer_copy->findOrigin (kernel));

				//Paranoid
				assert (copy != NULL);	

				//Ok now create the copy of the instruction we care about
				Instruction *inst = dynamic_cast<Instruction*> (interest->copy ());

				//Paranoid
				assert (inst != NULL);

				//Swap its operands
				inst->swapOperands ();

				//Replace instruction
				copy->replaceStatement (inst, i);                

				kernels.push_back (outer_copy);    

				//Recursive starting at i + 1 to generate every possible version
				handleSwap (kernels, outer_copy, copy, i + 1);
			}
		}
	}
}
