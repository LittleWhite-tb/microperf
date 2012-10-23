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
  @file StrideSelection.cpp
  @brief The StrideSelection pass is in this file 
 */

#include <cassert>
#include <sstream>

#include "Description.h"
#include "InductionOperand.h"
#include "Instruction.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"
#include "StrideSelection.h"

StrideSelection::StrideSelection (void)
{
	name = "Stride selection";
}

StrideSelection::~StrideSelection (void)
{

}

std::vector <PassElement *> *StrideSelection::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Stride selection", NULL);

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
	newKernels.clear ();

	Logging::log (0, "Stopping Stride selection", NULL);

	(void) desc;

	return res;
}

void StrideSelection::handlePass (std::vector<Kernel *> &kernels) const
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

void StrideSelection::handleKernel (std::vector<Kernel *> &kernels, const Kernel *outer, Kernel *kernel) const
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished
	if (kernel->getColor () == 0)
	{
		//Color this Kernel
		kernel->setColor (1);

		//Now handle this Kernel
		handleKernelChooseStride (kernels, outer, kernel);
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

void StrideSelection::handleKernelChooseStride (std::vector<Kernel *> &kernels, const Kernel *outer, const Kernel *kernel) const
{
	//For the moment, we have a simple selector, just get every instruction
	unsigned int nbr = kernel->getNbrInductions ();

	//We have nothing to do regarding Induction variables if nbr is == 0
	if (nbr != 0)
	{
		std::vector<const InductionOperand*> segment;

		//Fill the vector
		for (unsigned i = 0; i < nbr; i++)
		{
			const InductionOperand *tmp = kernel->getInduction (i);

			segment.push_back (tmp);
		}

		//For every kernel here, populate with this segment
		fillKernels (kernel, outer, kernels, segment);

		//Clear segment
		segment.clear ();
	}
}

void StrideSelection::fillKernels (const Kernel *kernel, const Kernel *outer, 
		std::vector<Kernel *> &newKernels, std::vector <const InductionOperand*> &segment) const
{
	std::ostringstream oss;
	oss << "Currently handling segment of size : " << segment.size ();
	Logging::log (0, oss.str ().c_str (), NULL);

	//Paranoid
	if (segment.size () != 0)
	{
		//A break down of the algorithm:
		//First thing is to go through the min/max stride of each induction variable, we will generate an array
		//Second, for this generation, we know how many of each instruction
		//Generate the randomize for it

		Logging::log (0, "Filling kernel with random induction variables", NULL);

		//1) Get the min/max for each instruction
		std::vector<SRandomRepeat> repetitions;
		for (std::vector<const InductionOperand *>::const_iterator it = segment.begin (); it != segment.end (); it++)
		{
			const InductionOperand *induction = *it;

			//Paranoid
			assert (induction != NULL);

			SRandomRepeat current;
			current.min = induction->getMinimumStride ();
			current.max = induction->getMaximumStride ();
			current.progress = induction->getProgressStride ();
			current.cur = current.min;

			repetitions.push_back (current);
		}

		//We automatically skip the first one: this is included in the dad's version
		getNextRepetition (repetitions);

		//Ok now we can generate all the repetitions
		while (repetitions.size () > 0 && repetitions[0].cur <= repetitions[0].max)
		{
			//Ok, now we have these repetitions, we can generate the benchmarks with these repetitions
			generateRandomKernels (kernel, outer, newKernels, segment, repetitions);

			//Get next repetition
			getNextRepetition (repetitions);
		}

		//Clear repetitions
		repetitions.clear ();
	}

	Logging::log (0, "Handled segment", NULL);
}

void StrideSelection::getNextRepetition (std::vector<SRandomRepeat> &repetitions) const
{
	//Get last index
	int cnt = repetitions.size () - 1;

	while (cnt >= 0)
	{  
		//Increment counter
		repetitions[cnt].cur += repetitions[cnt].progress;

		//Check max
		if (repetitions[cnt].cur > repetitions[cnt].max)
		{
			//If cnt == 0, it's over, we can leave it like this
			if (cnt == 0)
				break;

			//Ok we went too far, set back to min and we will do the next value
			repetitions[cnt].cur = repetitions[cnt].min;
		}
		else
		{
			//Otherwise, we are done here
			break;
		}
		cnt--;
	}
}

void StrideSelection::generateRandomKernels (const Kernel *kernel_orig, const Kernel *outer, std::vector<Kernel *> &newKernels,
		std::vector<const InductionOperand*> &segment,
		std::vector<SRandomRepeat> &repetitions) const
{
	Kernel *outer_copy = dynamic_cast <Kernel*> (outer->copy ());

	//Paranoid
	assert (outer_copy!= NULL);

	//Find our kernel
	Kernel *kernel = dynamic_cast<Kernel*> (outer_copy->findOrigin (kernel_orig));

	//Paranoid
	assert (kernel != NULL);

	//Get induction variables
	std::vector<InductionOperand *> copy_segment;

	unsigned int cnt = 0;
	for (std::vector<const InductionOperand*>::const_iterator it = segment.begin (); it != segment.end (); it++)
	{
		const InductionOperand *elem = *it;

		InductionOperand *copy = dynamic_cast<InductionOperand*> (elem->copy ());

		//Paranoid
		assert (cnt < repetitions.size ());

		copy->setStride (repetitions[cnt].cur);

		copy_segment.push_back (copy);
		cnt++;
	}

	//Copy the linked information
	kernel->handleTheirInductionLinks (copy_segment);

	//Now replace induction variables
	kernel->replaceInductionVariables (copy_segment);

	//Handle our induction variables
	kernel->handleInductionVariables ();

	//Clear vector
	copy_segment.clear ();

	//Now push this into newKernels
	newKernels.push_back (outer_copy);
}

