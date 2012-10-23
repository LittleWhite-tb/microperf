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
  @file StatementSelection.cpp
  @brief The StatementSelection pass is in this file 
 */

#include "Description.h"
#include "Instruction.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"
#include "Statement.h"
#include "StatementSelection.h"

#include <cassert>
#include <map>
#include <sstream>
#include <iostream>

enum KERNEL_COLOR {NONE = 0, VISITED = 1, READY = 2};

StatementSelection::StatementSelection (void)
{
	name = "Statement scheduling";
}

StatementSelection::~StatementSelection (void)
{
}

std::vector <PassElement *> *StatementSelection::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Statement Selection", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//Create new vector of kernels
	std::vector<Kernel*> newKernels;

	//Get original kernel
	Kernel *kernel = pe->getKernel ();

	if (kernel == NULL)
	{
		const Kernel *description = desc->getKernel ();

		kernel = dynamic_cast<Kernel *> (description->copy ());
	}
	assert (kernel != NULL);

	kernel->clearColor ();

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

	Logging::log (0, "Stopping Statement Selection", NULL);

	(void) desc;

	return res;
}

bool StatementSelection::kernelsAllColored (const Kernel *kernel, unsigned int value) const
{
	if (kernel->getColor () != value)
	{
		return false;
	}

	//Now go into the kernel
	unsigned int nbr = kernel->getNbrStatements ();

	//Now go into this Kernel and find the inner kernels
	for (unsigned int i = 0; i < nbr; i++)
	{
		//Get the instruction
		const Statement *stmt = kernel->getStatement (i);

		//If it's a Kernel, go into it
		const Kernel *inner = dynamic_cast<const Kernel *> (stmt);

		if (inner != NULL)
		{
			bool res = kernelsAllColored (inner, value);

			if (res == false)
			{
				return false;
			}
		}
	}

	return true;
}

void StatementSelection::handlePass (std::vector<Kernel *> &kernels) const
{
	unsigned int i = 0;

	std::vector<Kernel *> actual_kernels, kernels_to_delete;

	//Careful we do this on purpose: kernels can change as we are going through this
	while (i < kernels.size ())
	{
		std::vector<Kernel*> newKernels;

		Kernel *kernel = kernels[i];

		//Now we handle this Kernel and generate all the Stride variations we want
		handleKernel (newKernels, kernel, kernel);


		for (std::vector<Kernel*>::const_iterator it = newKernels.begin (); it != newKernels.end (); it++)
		{
			Kernel *kernel = *it;

			kernels.push_back (kernel);

			if (kernelsAllColored (kernel, READY) == true)
			{
				actual_kernels.push_back (*it);
			}
			else
			{
				kernels_to_delete.push_back (*it);
			}
		}
		newKernels.clear ();

		//Increment i
		i++;
	}

	//Free unused kernels
	for (std::vector<Kernel*>::const_iterator it = kernels_to_delete.begin (); it != kernels_to_delete.end (); it++)
	{
		Kernel *kernel = *it;
		delete kernel, kernel = NULL;
	}

	//Clear kernels
	kernels.clear ();

	kernels = actual_kernels;
}

void StatementSelection::handleKernel (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel) const
{
	//Handle whole kernel and find all the non colored kernels and pass them on

	//Check if we are finished or if we have an instructionMDL
	if (kernel->getColor () == NONE)
	{
		//Color this Kernel
		kernel->setColor (VISITED);

		//Now handle this Kernel
		handleSelection (kernels, outer, kernel);
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

void StatementSelection::handleSelection (std::vector<Kernel*> &kernels, const Kernel *outer, const Kernel *kernel) const
{
	//Create new Kernel vector
	std::vector<Kernel *> newKernels;

	//Now fill this vector with variants of this Kernel
	fillKernels (newKernels, kernel);

	//Ok we have work to do
	for (std::vector<Kernel*>::const_iterator it = newKernels.begin (); it != newKernels.end (); it++)
	{
		if (outer == kernel)
		{
			//Add to global list
			kernels.push_back (*it);
		}
		else
		{
			Kernel *outer_copy = dynamic_cast<Kernel*> (outer->copy ());

			//Paranoid
			assert (outer_copy != NULL);

			//Find the kernel
			Kernel *copy = dynamic_cast<Kernel*> (outer_copy->findOrigin (kernel));

			//Paranoid
			assert (copy != NULL);

			copy->setColor (READY);

			//Get variant
			Kernel *newVersion = *it;

			//Replace it
			assert (outer_copy->replaceStatement (newVersion, copy) == true);

			//Add to global list
			kernels.push_back (outer_copy);
		}
	}

	//Clear vector
	newKernels.clear ();
}

void StatementSelection::fillKernels (std::vector<Kernel *> &kernels, const Kernel *kernel) const
{
	std::ostringstream oss;

	//Paranoid
	assert (kernel != NULL);

	oss << "Currently handling kernel of size : " << kernel->getNbrStatements ();
	Logging::log (0, oss.str ().c_str (), NULL);

	//If this kernel is without a randomizer
	if (kernel->getRandomize () == false)
	{
		fillKernelsWithStatements (kernels, kernel);
	}
	else
	{
		bool combination = kernel->getCombination ();

		//Otherwise we are randomizing, this is more difficult, let's sub function it
		fillKernelsRandomize (kernels, kernel, combination);
	}

	Logging::log (0, "Handled Kernel", NULL);
}

void StatementSelection::fillKernelsWithStatements (std::vector<Kernel *> &kernels, const Kernel *kernel) const
{
	std::vector<Kernel *> todo, current;

	Logging::log (0, "Filling kernels with instructions", NULL);

	//For each instruction
	unsigned int nbr = kernel->getNbrStatements ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		//Get the instruction
		const Statement *inst = kernel->getStatement (i);

		assert (inst != NULL);

		//If current is empty
		if (current.size () == 0)
		{
			Kernel *copy = dynamic_cast<Kernel *> (kernel->copy ());

			//Paranoid
			assert (copy != NULL);

			copy->clearStatements ();

			copy->setColor (READY);

			//Create it from nothing
			handleStatement (inst, copy, current);

			//Delete the kernel now
			delete copy, copy = NULL;
		}
		else
		{
			//Otherwise, we handle this instruction and add it to the vector of Kernel

			//Copy current into todo
			todo = current;

			//Clear current
			current.clear ();

			//For each kernel
			for (std::vector<Kernel*>::const_iterator it_kernel = todo.begin (); it_kernel != todo.end (); it_kernel++)
			{
				Kernel *kernel = *it_kernel;

				handleStatement (inst, kernel, current);

				//Delete the kernel now
				delete kernel, kernel = NULL;
			}
		}
	}

	//Now add current to the list
	kernels = current;
}

void StatementSelection::handleStatement (const Statement *stmt, Kernel *kernel, std::vector<Kernel *> &list) const
{
	for (unsigned int i = stmt->getMinRepeat (); i <= stmt->getMaxRepeat (); i += stmt->getProgressRepeat ())
	{
		//Create new kernel
		Kernel *current = NULL;

		if (kernel != NULL)
		{
			current = dynamic_cast<Kernel*> (kernel->copy ());
		}
		else
		{
			assert (0);
		}

		assert (current != NULL);

		//Now generate the number of repeats
		for (unsigned int j = 0; j < i; j++)
		{
			Statement *copy = dynamic_cast<Statement*> (stmt->copy ());

			//Paranoid
			assert (copy != NULL);

			copy->updateRegisterName (j);
			current->addStatement (copy);
		}

		//Now add to the list
		list.push_back (current);
	}
}

void StatementSelection::fillKernelsRandomize (std::vector<Kernel *> &kernels, const Kernel *kernel, bool combination) const
{
	//Randomize means we modify the order, we suppose for the moment all of the instructions must be present

	//A break down of the algorithm:
	//First thing is to go through the min/max repeats of each instruction, we will generate an array
	//Second, for this generation, we know how many of each instruction
	//Generate the randomize for it

	Logging::log (0, "Filling kernels with random instructions", NULL);

	//1) Get the min/max for each instruction
	std::vector<SRandomRepeat> repetitions;

	unsigned int nbr = kernel->getNbrStatements ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		const Statement *stmt = kernel->getStatement (i);
		//Paranoid
		assert (stmt != NULL);

		SRandomRepeat current;
		current.min = stmt->getMinRepeat ();
		current.max = stmt->getMaxRepeat ();
		current.progress = stmt->getProgressRepeat ();
		current.cur = current.min;

		repetitions.push_back (current);
	}

	//Create new vector for kernels
	std::vector<Kernel*> newKernels;

	//Ok now we can generate all the repetitions
	while (repetitions.size () > 0 && repetitions[0].cur <= repetitions[0].max)
	{
		//Ok, now we have these repetitions, we can generate the benchmarks with these repetitions
		generateRandomKernels (kernels, newKernels, kernel, repetitions, combination);

		//Get next repetition
		getNextRepetition (repetitions);
	}

	//Clear repetitions
	repetitions.clear ();

	//Free the old kernels
	for (std::vector<Kernel *>::const_iterator it = kernels.begin (); it != kernels.end (); it++)
	{
		Kernel *kernel = *it;
		delete kernel, kernel = NULL;
	}
	kernels.clear ();

	//Copy over
	kernels = newKernels;
	newKernels.clear ();
}

void StatementSelection::getNextRepetition (std::vector<SRandomRepeat> &repetitions) const
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

void StatementSelection::generateRandomKernels (std::vector<Kernel *> &kernels, std::vector<Kernel *> &newKernels, const Kernel *kernel,
		std::vector<SRandomRepeat> &repetitions, bool combination) const
{
	//Ok first thing is actually to have the number of instructions we are generating
	unsigned int cnt = 0;
	for (std::vector<SRandomRepeat>::const_iterator it = repetitions.begin (); it != repetitions.end (); it++)
	{
		const SRandomRepeat &rep = *it;
		cnt += rep.cur;
	}

	unsigned int *permutations = new unsigned int[cnt];

	//Now we go into our permutation code for these instructions
	generateRandomKernelsPermute (kernels, newKernels, kernel, repetitions, permutations, cnt, 0, combination);

	delete[] permutations, permutations = NULL;
}

void StatementSelection::generateRandomKernelsPermute (std::vector<Kernel *> &kernels, std::vector<Kernel *> &newKernels, const Kernel *kernel,
		std::vector<SRandomRepeat> &repetitions, unsigned int *permutations,
		unsigned int max, unsigned int current,
		bool combination) const
{
	//If we are done, we have a valid permutation
	if (current == max)
	{
		fillRandomKernel (kernels, newKernels, kernel, repetitions, permutations, max);
	}
	else
	{
		//Otherwise, we have a bit of work
		unsigned int i = 0;

		while (i < max)
		{
			unsigned int j;

			//Check if it hasn't been used yet
			for (j = 0; j < current; j++)
			{
				if (permutations[j] == i)
				{
					break;
				}
			}

			//If not yet used, we can use it here
			if (j == current)
			{
				//Set it
				permutations[current] = i;

				//Call the next one
				generateRandomKernelsPermute (kernels, newKernels, kernel, repetitions, permutations, max, current + 1, combination);

				//We accepted this one so we can go to the next instruction (if this is our mode)
				i = findNextElement (repetitions, i, combination);
			}
			else
			{
				//We didn't accept this one, so let's try the next one
				i++;
			}
		}
	}
}

unsigned int StatementSelection::findNextElement (std::vector<SRandomRepeat> &repetitions, unsigned int current, bool combination) const
{
	//If we are in combination mode, we have a bit of work
	if (combination == true)
	{
		unsigned int value = current;

		//Now find out which instruction this is
		unsigned int j = findStatement (repetitions, value, true);
		assert (j < repetitions.size ());

		//Ok now what we want is to find out where we are compared to this one
		unsigned int start = current - value;
		unsigned int complement = repetitions[j].max - repetitions[j].min + 1;

		return start + complement;
	}
	else
	{
		//Otherwise we just return current + 1
		return current + 1;
	}
}

void StatementSelection::fillRandomKernel (std::vector<Kernel *> &kernels, std::vector<Kernel *> &newKernels, const Kernel *kernel,
		std::vector<SRandomRepeat> &repetitions, unsigned int *permutations, unsigned int max) const
{
	std::map<const Statement *, unsigned int> copyVersions;
	std::vector<const Statement *>list;

    //Go through each permutation
	for (unsigned int i = 0; i < max; i++)
	{
		//Get value
		unsigned int value = permutations[i];

		unsigned int j = findStatement (repetitions, value);
		assert (j < repetitions.size ());

		//Now we can add it to every kernel
		const Statement *orig = kernel->getStatement (j);

		//Check if it exists in the map
		unsigned int version = 0;

		if (copyVersions.find (orig) != copyVersions.end ())
		{
			version = copyVersions[orig];
			version++;
		}
		//Set it back now (or 0 if not found)
		copyVersions[orig] = version;

		//Copy it with version
		Statement *inst = dynamic_cast<Statement*> (orig->copy ());

		//Paranoid
		assert (inst != NULL);

		//Update names
		inst->updateRegisterName (version);

		//Add to list
		list.push_back (inst);
	}

	//If we have a kernels already generated
	if (kernels.size () > 0)
	{
		//Ok, now we have our list of randomized instructions, we can add them to the kernels
		for (std::vector<Kernel *>::const_iterator it = kernels.begin (); it != kernels.end (); it++)
		{
			Kernel *kernel = *it;

			//First create a new kernel
			Kernel *current = dynamic_cast<Kernel *> (kernel->copy ());

			//Paranoid
			assert (current != NULL);

			current->clearStatements ();

			current->setColor (READY);

			//Now copy in the new instructions
			unsigned int nbr = kernel->getNbrStatements ();
			for (unsigned int j = 0; j < nbr; j++)
			{
				const Statement *stmt = kernel->getStatement (j);
				current->addStatement (stmt);
			}

			//Now copy in our list
			for (std::vector<const Statement *>::const_iterator it_list = list.begin (); it_list != list.end (); it_list++)
			{
				const Statement *inst = *it_list;
				current->addStatement (inst);
			}

			//Now push this into newKernels
			newKernels.push_back (current);
		}
	}
	else
	{
		//No kernels generated, this is the first time
        //First create a new kernel
        Kernel *current = dynamic_cast<Kernel *> (kernel->copy ());

        //Paranoid
        assert (current != NULL);

        current->clearStatements ();

        current->setColor (READY);

		//Now copy in our list
		for (std::vector<const Statement *>::const_iterator it_list = list.begin (); it_list != list.end (); it_list++)
		{
			const Statement *inst = *it_list;
			current->addStatement (inst);
		}

		//Now push this into newKernels
		newKernels.push_back (current);
	}

	//Free map and list
	copyVersions.clear ();
	for (std::vector<const Statement *>::const_iterator it = list.begin (); it != list.end (); it++)
	{
		const Statement *inst = *it;
		Statement *todo = (Statement*) inst;
		delete todo, todo = NULL, inst = NULL;
	}
	list.clear ();
}

unsigned int StatementSelection::findStatement (std::vector<SRandomRepeat> &repetitions, unsigned int &search, bool modifyValue) const
{
	unsigned int value = search;

	//Now find out which instruction this is
	unsigned int j;
	for (j = 0; j < repetitions.size (); j++)
	{
		//If it's this one
		if (value < repetitions[j].cur)
		{
			break;
		}

		//Otherwise, we subtract the counter
		value -= repetitions[j].cur;
	}

	if (modifyValue == true)
		search = value;

	return j;
}
