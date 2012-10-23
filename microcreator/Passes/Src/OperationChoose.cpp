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
  @file OperationChoose.cpp
  @brief The OperationChoose pass is in this file 
 */

#include <cassert>
#include <fstream>

#include <iostream>

#include "Description.h"
#include "Instruction.h"
#include "Operation.h"
#include "PassElement.h"
#include "Kernel.h"
#include "Logging.h"
#include "OperationChoose.h"

OperationChoose::OperationChoose (bool bu)
{
	beforeUnroll = bu;

    //Set name depending on bu
	if (bu)
	{
		name = "Operation choose before unroll";
	}
	else
	{
		name = "Operation choose after unroll";
	}
}

OperationChoose::~OperationChoose (void)
{
}

std::vector <PassElement *> *OperationChoose::entry (PassElement *pe, Description *desc) const 
{
	Logging::log (0, "Starting Operation Choose", NULL);

	//First get Kernel
	Kernel *kernel = pe->getKernel ();

	if (kernel == NULL)
	{
		const Kernel *description = desc->getKernel ();

		kernel = dynamic_cast<Kernel *> (description->copy ());
	}

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
			pe_tmp = new PassElement ();
		}

		pe_tmp->setKernel (tmp);
		res->push_back (pe_tmp);
	}

	//Delete the first kernel
	if (res->size () > 1)
	{
		if ((res->front ())->getKernel () == kernel)
		{
			//res->erase (res->begin ());
		}
	}

	Logging::log (0, "Stopping Operation Choose", NULL);

	(void) desc;

	return res;
}        

void OperationChoose::handlePass (std::vector<Kernel *> &kernels) const
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

void OperationChoose::handleKernel (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel) const
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		//Now handle this Kernel
		handleOpChoose (kernels, outer, kernel);
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

void OperationChoose::handleOpChoose (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel, unsigned int start) const
{	
	unsigned int nbr = kernel->getNbrStatements ();

	for (unsigned int i = start; i < nbr; i++)
	{
		//Get instruction
		Statement *s_interest = kernel->getModifiableStatement (i);

		//Paranoid
		assert (s_interest != NULL);

		//We only care about instructions
		Instruction *interest = dynamic_cast<Instruction *> (s_interest);

		if (interest != NULL)
		{
			bool test = false;

			if (beforeUnroll)
			{
				test = (interest->getChooseOpAfter () == false);
			}		
			else
			{
				test = interest->getChooseOpAfter ();
			}

			if (test == true)
			{
				std::vector <Operation*> vect = interest->getOperationVect ();

                //For each Operation to choose from
				for (std::vector <Operation*>::const_iterator it = vect.begin (); it != vect.end (); it++)
				{
					//Create a new kernel
					Kernel *outer_copy = dynamic_cast<Kernel*> (outer->copy ());

					//Paranoid
					assert (outer_copy != NULL);

					//Now find our Kernel
					Kernel *copy = dynamic_cast<Kernel*> (outer_copy->findOrigin (kernel));

					//Paranoid
					assert (copy != NULL);

					//Create a new instruction
					Instruction* inst = dynamic_cast <Instruction*> (interest->copy ());

					//Paranoid
					assert (inst != NULL);

					//Add the operation
					inst->setOperation (*it);

					//Replace instruction
					copy->replaceStatement (inst, i);

					kernels.push_back (outer_copy);

					//Handle the copy
					handleOpChoose (kernels, outer_copy, copy, i + 1);
				}
			}
		}
	}
}
