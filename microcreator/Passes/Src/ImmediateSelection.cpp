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
  @file ImmediateSelection.cpp
  @brief The ImmediateSelection pass is in this file 
 */

#include <iostream>
#include <cassert>

#include "Description.h"
#include "ImmediateOperand.h"
#include "ImmediateSelection.h"
#include "Instruction.h"
#include "InsertCode.h"
#include "Kernel.h"
#include "Logging.h"
#include "Operand.h"
#include "PassElement.h"
#include "Statement.h"

ImmediateSelection::ImmediateSelection (bool bu)
{
	beforeUnroll = bu;

    //Update pass name
	if (bu == true)
	{
		name = "Immediate selection before unroll";		
	}
	else
	{
		name = "Immediate selection after unroll";
	}
}

ImmediateSelection::~ImmediateSelection (void)
{

}

std::vector <PassElement *> *ImmediateSelection::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Operand Immediate", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//Push in the vector our original one
	res->push_back (pe);

	//Ok, get the original kernel
	Kernel *orig = pe->getKernel ();

	//If we have work
	if (orig != NULL)
	{
		handleImmediate (res, orig, orig);
	}
	Logging::log (0, "Stopping Operand Immediate", NULL);

	(void) desc;

	return res;
}

void ImmediateSelection::handleImmediate (std::vector <PassElement *> *pool, const Kernel *outer, Kernel *kernel, unsigned int start) const
{
	//Paranoid
	assert (pool != NULL);

	//Paranoid
	if (kernel == NULL || kernel->getNbrStatements () <= start)
	{
		return;
	}

    //How many statements do we have?
	unsigned int nbr = kernel->getNbrStatements ();

    //For each statement from start to the end
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
			//Update the immediate operand depending on the beforeUnroll member
			bool test = false;

            //Test says if we are interested in it or not
			if (beforeUnroll == true)
			{
				test = (interest->getImmediateAfter () == false);
			}		
			else
			{
				test = interest->getImmediateAfter ();
			}

			if (test == true)
			{
				unsigned int nbOp = interest->getNbrOperands();
				for (unsigned int j = 0; j < nbOp; j++) 
				{
					//Get operand
					Operand *s_op = interest->getModifiableOperand (j);

					//we only care about immediate operand
					ImmediateOperand *immediateOp = dynamic_cast<ImmediateOperand *> (s_op);

					if (immediateOp != NULL)
					{
						std::string slink = "";
						slink = interest->getLinked ();

						//Handle the linked instruction
						if (slink != "")
						{
							const Instruction *linked = findOurLinked (outer, interest->getLinked ());

							//Now, we can update the immediate operand	
							updateImmediateOperand (linked, immediateOp);
						}

						long value = immediateOp->getValImmediate(), 
                             min = immediateOp->getMinImmediate(), 
                             max = immediateOp->getMaxImmediate();
						long progress = immediateOp->getProgressImmediate();

						if ((min > max) || (progress <= 0))
						{
							Logging::log (2, "Error: Wrong parameters from the immediate operand", NULL);
							return;
						}

                        //If we have no value, we actually have a range
						if (value == -1)
						{												
                            //It's min + progress because min is handled later
							for (int k = min + progress; k <= max; k += progress)	  	
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

								//Update the operand
								immediateOp->setValImmediate (k);

								//We create a copy of the operand we care about
								ImmediateOperand *op = dynamic_cast<ImmediateOperand *> (immediateOp->copy ());

								//Choose my value
								op->setValImmediate (k);								

								//Now we update the operand of the instruction
								inst->setOperand (j, op);								

								//Replace instruction with my new instruction
								copy->replaceStatement (inst, i); 

								//Finally create the new PassElement
								PassElement *newElement = new PassElement ();
								newElement->setKernel (outer_copy);

								//Add to newElements
								pool->push_back (newElement);

								//Now we call the handleImmediate with a new start index
								handleImmediate (pool, outer_copy, outer_copy);        		  							
							}	

							//Min is the first immediate value, we said we'd handle it ;-)
							immediateOp->setValImmediate (min);
						}
					}          		
				}	        
			}
		}   
		else
		{        								
			//If it's a kernel we have to start over
			Kernel *inner = dynamic_cast<Kernel *> (s_interest);

			if (inner != NULL)
			{
				//Actually, just update what we're doing
				handleImmediate (pool, outer, inner);
			}
		}                     
	}
}

const Instruction *ImmediateSelection::findOurLinked (const Kernel *outer, const std::string &slinked) const
{
	//If no name, or outer is NILL 
	if ((slinked == "") || (outer == NULL))
	{
		return NULL;
	}

	unsigned int nbr = outer->getNbrStatements ();
	for (unsigned int i = 0; i < nbr; i++)
	{
		//Get instruction
		const Statement *stmt = outer->getStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//We only care about instructions
		const Instruction *inst = dynamic_cast<const Instruction *> (stmt);

		if (inst != NULL)
		{		
			//Check this name
			if (inst->getName () == slinked)
			{
				return inst;
			}
		}
		else
		{
            //If it is not, it might be a kernel and then we have to do a recursive search
			const Kernel *inner = dynamic_cast<const Kernel *> (stmt);
            
			const Instruction *inst_tmp = NULL;
			if (inner != NULL)
			{
				inst_tmp = findOurLinked (inner, slinked);	

				if (inst_tmp != NULL)
				{
					return inst_tmp;
				}
			}
		}
	}	

	return NULL;
}

void ImmediateSelection::updateImmediateOperand (const Instruction *linked, ImmediateOperand *operand) const
{
	//If we have a linked immediate operand
	if (linked != NULL)	
	{
		//Get the immediate value
		for (unsigned int m = 0; m < linked->getNbrOperands(); m++) 
		{
			//Get operand
			const Operand *op = linked->getOperand (m);

			//we only care about immediate operand
			const ImmediateOperand *immOp = dynamic_cast<const ImmediateOperand *> (op);

			if (immOp != NULL)
			{
				//Update with new immediate operand information
				operand->setValImmediate (immOp->getValImmediate ());
			}
		}
	}
}
