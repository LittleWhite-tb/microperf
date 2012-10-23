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
  @file PassUnroll.cpp
  @brief The PassUnroll pass is in this file 
 */

#include <iostream>
#include <cassert>
#include <sstream>

#include "Comment.h"
#include "Description.h"
#include "Instruction.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"
#include "PassUnroll.h"

PassUnroll::PassUnroll (void)
{
	name = "Pass unroll";
}

PassUnroll::~PassUnroll (void)
{

}

std::vector <PassElement *> *PassUnroll::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Unroll", NULL);

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
			pe_tmp = new PassElement ();
		}
		pe_tmp->setKernel (tmp);
		res->push_back (pe_tmp);
	}

	Logging::log (0, "Stopping Unroll", NULL);

	(void) desc;	

	return res;
}

void PassUnroll::handlePass (std::vector<Kernel *> &kernels) const
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

void PassUnroll::handleKernel (std::vector<Kernel*> &kernels, Kernel *outer, Kernel *kernel) const
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		std::string linked = "";
		linked = kernel->getLinked ();

		//Now handle this Kernel
		handleUnroll (kernels, outer, kernel);
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

bool PassUnroll::kernelContainsInstructions (const Kernel *kernel) const
{
	int nbr = kernel->getNbrStatements ();

	for (int i = 0; i < nbr; i++)
	{
		const Statement *stmt = kernel->getStatement (i);

		const Instruction *inst = dynamic_cast<const Instruction*> (stmt);

		if (inst != NULL)
			return true;
	}

	//No instruction was found
	return false;
}

const Kernel *PassUnroll::findOurLinked (const Kernel *outer, const std::string &slinked) const
{
	//If no name, or outer is NILL 
	if ((slinked == "") || (outer == NULL))
	{
		return NULL;
	}

	//Check this name
	if (outer->getName () == slinked)
	{
		return outer;
	}

    //Not the right name, look for any internal kernels
	for (unsigned int i = 0; i < outer->getNbrStatements (); i++)
	{
		const Statement *stmt = outer->getStatement (i);
		const Kernel *kernel = dynamic_cast<const Kernel*> (stmt);
		
		const Kernel *kernel_tmp = NULL;

		if (kernel != NULL)
		{
			kernel_tmp = findOurLinked (kernel, slinked);

			if ((kernel_tmp != NULL) && (kernel_tmp->getName () == slinked))
			{
				return kernel_tmp;
			}
		}
	}
	return NULL;
}

void PassUnroll::handleUnroll (std::vector<Kernel*> &kernels, Kernel *outer, Kernel *kernel) const
{
	//if the kernel was a linked one or not?
	bool linkedKernel = false;

	const Kernel *linked = findOurLinked (outer, kernel->getLinked ());

	//If we have a linked Kernel
	if (linked != NULL)	
	{
		//Then we don't care about our unroll information, let's copy its unroll information using the actual unroll
		unsigned int actualUnroll = linked->getActualUnroll ();
		kernel->setMinUnroll (actualUnroll);	
		kernel->setMaxUnroll (actualUnroll);	
		kernel->setProgressUnroll (1);

		linkedKernel = true;
	}

	//Now get unroll information
	int min = kernel->getMinUnroll (), 
        max = kernel->getMaxUnroll (), 
        progress = kernel->getProgressUnroll ();	

	if ((min > max || min < 0) || (progress == 0))
	{
		Logging::log (2, "Error: Wrong parameters for the unroll", NULL);
		return;
	}

	//If something to do here
	if (min != 1 || max != 1)
	{       	    
		//Otherwise, we have a bit of work: min + progress because we handle the minimum afterwards
		for (int i = min + progress; i <= max; i+=progress)
		{
			//We have nothing to do in the case of 1 because our dad did it for us
			if (i == 1)
			{
				continue;
			}

			unrollIt (kernels, outer, kernel, i, true, linkedKernel);
		}

		//Ok now check if we want to unroll the minimum
		if (min > 1)
		{
			unrollIt (kernels, outer, kernel, min, false, linkedKernel);
		}                      
	}
	else
	{
		//In this case, add unroll comment now, it'll copy itself to all new unrolls
		if (kernelContainsInstructions (kernel) == true)
		{
			std::ostringstream oss;
			oss << "Did not Unroll from " << min << " to " << max;
			Comment *unrollCom = new Comment (oss.str ());
			kernel->addStatement (unrollCom);   	
		}
	}

	//Adding a comment for minimum 1
	if (min == 1)
	{
		if (kernelContainsInstructions (kernel) == true)
		{
			std::ostringstream oss;
			oss << "Unrolled factor " << min;
			Comment *unrollCom = new Comment (oss.str ());
			kernel->addStatement (unrollCom);   	
		}
	}
}

void PassUnroll::unrollIt (std::vector<Kernel*> &kernels, Kernel *outer, Kernel *kernel, int i, bool unroll, bool linkedKernel) const
{
	std::ostringstream oss;
	std::string comStr;
	Kernel *outer_copy = NULL;
	Kernel *copy = NULL;
	Kernel *tmp = NULL;

	//Create a copy of the outer
	outer_copy = dynamic_cast<Kernel*> (outer->copy ());

	//Paranoid
	assert (outer_copy != NULL);

	if (unroll == true)    
	{
		//Find the kernel
		copy = dynamic_cast<Kernel*> (outer_copy->findOrigin (kernel));

		//Paranoid
		assert (copy != NULL);

		//Now remove instructions
		copy->clearStatements ();
	}
	else 
	{
		tmp = dynamic_cast<Kernel*> (kernel->copy ());

		copy = kernel;

		//Paranoid
		assert (copy != NULL);

		copy->clearStatements ();
	}	    

	Comment *unrollCom = new Comment ("Unroll beginning");
	copy->addStatement (unrollCom);   	

	if (kernelContainsInstructions (kernel) == true)
	{
		std::ostringstream oss;
		oss << "Unrolled factor " << i;
		Comment *unrollCom = new Comment (oss.str ());
		copy->addStatement (unrollCom);   	
	}

	for (int j = 0; j < i; j++)
	{
		unsigned int nbr = copy->getNbrStatements ();

		oss.str("");
		comStr.clear ();
		oss << (j+1) << " out of "<< i;
		comStr = "Unrolling, iteration " + oss.str () ;
		Comment *unrollCom = new Comment (comStr);
		copy->addStatement (unrollCom);

		//Append first
		if (unroll)        
		{
			copy->append (kernel);
		}
		else 
		{
			copy->append (tmp);	
		}

		//Now inform we are unrolling the other instructions and which unrolled iteration they are
		for (unsigned int k = nbr; k < copy->getNbrStatements (); k++)
		{
			Statement *stmt = copy->getModifiableStatement (k);

			stmt->updateUnrollInformation (j);

			stmt->updateRegisterName (j);
		}
	}

	if (i > 1)
	{
		//Update Induction Unrolling
		copy->updateInductionUnrolling (i);

		//Update the actual unroll
		copy->setActualUnroll (i);
	}

	Comment *endInst = new Comment ("Unroll ending");
	copy->addStatement (endInst);

	if (unroll == true)	
	{		
		kernels.push_back (outer_copy);
	}   

	//Free them
	if (linkedKernel == true)
	{
		delete tmp, tmp = NULL;
		delete outer_copy, outer_copy = NULL;
	}
}

