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
  @file InductionSelection.cpp
  @brief The InductionSelection pass is in this file 
 */

#include <cassert>

#include "Comment.h"
#include "Description.h"
#include "InductionSelection.h"
#include "Instruction.h"
#include "InsertCode.h"
#include "Kernel.h"
#include "Logging.h"
#include "Operand.h"
#include "PassElement.h"
#include "Statement.h"

InductionSelection::InductionSelection (void)
{
	name = "Induction selection";
}

InductionSelection::~InductionSelection (void)
{
}

std::vector <PassElement *> *InductionSelection::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Induction Selection", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//the kernel of the passElement
	Kernel* kernel = pe->getKernel ();

	if(kernel != NULL)
	{
		handleKernel (kernel);
	}

	//Add this kernel now to res
	res->push_back (pe);

	Logging::log (0, "Stopping Induction Selection", NULL);

	(void) desc;

	return res;
}

void InductionSelection::handleKernel (Kernel *ker) const
{
	//Now handle each instruction
	unsigned int nbr = ker->getNbrStatements ();

	for(unsigned int i = 0; i < nbr; i++)
	{
		Statement *inst = ker->getModifiableStatement (i);

		handleStatement (inst, ker);
	}
}

void InductionSelection::handleStatement (Statement *stmt, Kernel* kernel) const
{
	//We only care about Instruction
	Instruction *inst = dynamic_cast<Instruction*> (stmt);

	if (inst != NULL)
	{
        //Now we only care about its operands
		unsigned int nbr = inst->getNbrOperands ();

		for(unsigned int i = 0; i < nbr; i++)
		{
			Operand *op = inst->getModifiableOperand (i);

            //Handle induction variables for the operand
			op->handleInductionVariables (kernel,false);
		}

		return;
	}

	//Now we care about Kernel too
	Kernel *inner = dynamic_cast<Kernel*> (stmt);

	if (inner != NULL)
	{
		handleKernel (inner);
		return;
	}

	//Now we care about InsertCode
	InsertCode *insert = dynamic_cast<InsertCode*> (stmt);

	if (insert != NULL)
	{
		//Just get out
		return;
	}

	//Now we care about Comment 
	Comment *comment = dynamic_cast<Comment*> (stmt);

	if (comment != NULL)
	{
		//Just get out
		return;
	}

	//Paranoid
	assert (0);
}
