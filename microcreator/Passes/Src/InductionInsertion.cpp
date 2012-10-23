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
  @file InductionInsertion.cpp
  @brief The InductionInsertion pass is in this file 
 */

#include <cassert>
#include <sstream>

#include "Comment.h"
#include "Description.h"
#include "InductionInsertion.h"
#include "InductionOperand.h"
#include "Instruction.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"

InductionInsertion::InductionInsertion (void)
{
	name = "Induction insertion";
}

InductionInsertion::~InductionInsertion (void)
{

}

std::vector <PassElement *> *InductionInsertion::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Induction insertion", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//The kernel of the passElement
	Kernel* kernel = pe->getKernel ();

    //Handle the kernel
	handleKernel (kernel);

	Logging::log (0, "Stopping Induction insertion", NULL);

	//Push back pe
	res->push_back (pe);

	(void) desc;

	return res;
}

void InductionInsertion::handleKernel (Kernel *kernel) const
{
	if(kernel != NULL)
	{
		//Now add the InductionOperands to the Kernel
		unsigned int nbr = kernel->getNbrInductions ();
		Instruction *lastInstruction = NULL;

		if (nbr > 0)
		{
			//Prepare comments
			std::string commentString = "Induction variables";
			std::ostringstream oss;

            //Add a comment
			Comment* beginComment = new Comment (commentString);
			kernel->addStatement (beginComment);

            //Now add the instuction variables
			for (unsigned int i = 0; i < nbr; i++)
			{
				const InductionOperand *induction = kernel->getInduction (i);

				//Paranoid
				assert (induction != NULL);

				//if allowed to be generated
				if (induction->getNoGenerated () == false)
				{
					//If last induction, remember it
					if (induction->getLastInduction () == true)
					{
                        //We only allow one last induction variable
						if (lastInstruction == NULL)
						{
							lastInstruction = induction->getInstruction ();
						}
						else
						{
							Logging::log (2, "Error: Multiple 'last' inductions variable", NULL);
							assert(0);
						}
					}
					else
					{
                        //Ok, we do want to generate it now
						std::ostringstream oss;
                        //First a comment
						oss << "#Induction variable: " << induction->getStride () << " , " << induction->getBaseIncrement () << " , " << induction->getBaseOffset ();

                        //Add comment and then the induction's instruction
						Comment *comment = new Comment (oss.str ());
						kernel->addStatement (comment);

						Instruction *instruction = induction->getInstruction ();
						kernel->addStatement (instruction);
					}	
				}
			}

            //If we have a last induction variable, add it now
			if (lastInstruction != NULL)
			{
				kernel->addStatement (lastInstruction);
			}
		}

		//Now handle the other internal Kernels
		nbr = kernel->getNbrStatements ();

		for (unsigned int i = 0; i < nbr; i++)
		{
			//Get the instruction
			Statement *stmt = kernel->getModifiableStatement (i);

			//If it's a Kernel, go into it
			Kernel *inner = dynamic_cast<Kernel *> (stmt);

			if (inner != NULL)
			{
				handleKernel (inner);
			}
		}
	}
	else
	{
		Logging::log (1, "Warning: Kernel is NULL for InductionInsertion", NULL);
	}
}
