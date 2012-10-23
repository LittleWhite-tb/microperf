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
 @file Instruction.cpp
 @brief The Instruction class is in this file
 */

#include <iostream>
#include <cassert>

#include "Description.h"
#include "Instruction.h"
#include "Operation.h"
#include "Operand.h"
#include "Logging.h"

Instruction::Instruction (void)
{
	swapBefore = false;
	swapAfter = false;
	combination = true;
	op = NULL;
	chooseOpBefore = false;
	chooseOpAfter = false;
	immediateBefore = false;
	immediateAfter = false;
}

Instruction::~Instruction (void)
{
	//Delete operation
	delete op, op = NULL;

	//Delete operand vector
	for (std::vector<Operand*>::const_iterator it = operands.begin (); it != operands.end (); it++)
	{
		Operand *tmp = *it;
		delete tmp, tmp = NULL;
	}
	operands.clear ();
}

void Instruction::fillInstruction (void)
{

}

const Operation* Instruction::getOperation (void) const
{
	return op;
}

Operation* Instruction::getModifiableOperation (void) const
{
	return op;
}

void Instruction::setOperation (Operation *operation)
{
	delete op, op = 0;	
	op = operation;
}

const Operand* Instruction::getOperand (unsigned int idx) const
{
	if (idx >= operands.size ())
		return NULL;
	return operands[idx];
}

void Instruction::setOperand (unsigned int idx, Operand *op)
{
	//If not valid, we don't do anything
	if (idx >= operands.size ())
		return;

	Operand *old = operands[idx];
	delete old, old = NULL;
	operands[idx] = op;
}

void Instruction::setComment (const std::string comment)
{
	this->comment = comment;
}

const std::string Instruction::getComment (void) const
{
	return comment;
}

Operand* Instruction::getModifiableOperand (unsigned int idx)
{
	if (idx >= operands.size ())
		return NULL;
	return operands[idx];
}

unsigned int Instruction::getNbrOperands (void) const
{
	return operands.size ();
}

void Instruction::addOperand (Operand *operand)
{
	if (operand != NULL)
		operands.push_back (operand);
}

void Instruction::setCombination (bool r)
{
	combination = r;
}

bool Instruction::getCombination (void) const
{
	return combination;
}

void Instruction::setSwapBefore (bool r)
{
	swapBefore = r;
}

bool Instruction::getSwapBefore (void) const
{
	return swapBefore;
}

void Instruction::setSwapAfter (bool r)
{
	swapAfter = r;
}

bool Instruction::getSwapAfter (void) const
{
	return swapAfter;
}

void Instruction::setChooseOpBefore (bool r)
{
	chooseOpBefore = r;
}

bool Instruction::getChooseOpBefore (void) const
{
	return chooseOpBefore;
}

void Instruction::setChooseOpAfter (bool r)
{
	chooseOpAfter = r;
}

bool Instruction::getChooseOpAfter (void) const
{
	return chooseOpAfter;
}

void Instruction::setImmediateBefore (bool r)
{
	immediateBefore = r;
}

bool Instruction::getImmediateBefore (void) const
{
	return immediateBefore;
}

void Instruction::setImmediateAfter (bool r)
{
	immediateAfter = r;
}

bool Instruction::getImmediateAfter (void) const
{
	return immediateAfter;
}

void Instruction::setOperationVect (std::vector<Operation*> vect)
{
	operationVect = vect;
}

std::vector<Operation*> Instruction::getOperationVect (void) const
{
	return operationVect;
}

bool Instruction::getString (std::string &s, const Description *desc, unsigned int tab) const
{
	setTabulations (s, tab);
	if (desc->getAsmVolatile () == true)
	{
		s += "asm volatile (\"";
	}

	//Second set operation
	const Operation *operation = getOperation ();

	if (operation != NULL)
	{
		operation->addString (s);
	}
	else
	{
		s = "NoOp";
	}

	//Add space
	s += " ";

	//Now for each operand, add it
	unsigned int nbr = getNbrOperands ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		const Operand *op = getOperand (i);

		op->addString (s);

		//Add separator if not last
		if (desc != NULL && i < nbr - 1)
		{
			desc->addSeparator (s);
		}
		else
		{
			if (desc == NULL)
			{
				s += " ";
			}
		}
	}

	if (desc->getAsmVolatile () == true)
	{
		s += "\");";
	}

	if (getComment ().size () > 0)
	{
		s += " # " + getComment ();
	}

	using namespace std;

	return true;
}

Statement *Instruction::copy (void) const
{
	Instruction *inst = new Instruction ();
	inst->copyInformation (this);

	//Link it up
	inst->origin = this;

	return inst;
}

void Instruction::copyInformation (const Statement *stmt)
{
	const Instruction *orig = dynamic_cast<const Instruction*> (stmt);

	//Paranoid
	assert (orig != NULL);

	//Copy any values that are not pointers
	*this = *orig;

	//Copy operation
	Operation *op_copy = NULL;
	const Operation *op_orig = orig->getOperation ();

	if (op_orig != NULL)
		op_copy = op_orig->copy ();

	//replace class variable
	op = op_copy;

	//Ok now free operands (we are going to copy them)
	operands.clear ();

	setComment (orig -> getComment ());

	unsigned int nbr = orig->getNbrOperands ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		const Operand *op = orig->getOperand (i);

		//Paranoid
		if (op != NULL)
		{
			Operand *copy = op->copy ();

			addOperand (copy);
		}
	}
}

void Instruction::swapOperands (void)
{
	//For the moment only handle a two operand instruction
	assert (operands.size () == 2);

	Operand *first = operands[0];
	Operand *second = operands[1];

	operands[0] = second;
	operands[1] = first;
}

void Instruction::updateUnrollInformation (int nbrIterationsAdvance)
{
	unsigned int nbr = getNbrOperands ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		Operand *op = getModifiableOperand (i);

		//Paranoid
		if (op != NULL)
		{
			op->updateUnroll (nbrIterationsAdvance);
		}
	}
}

void Instruction::updateRegisterName (int nbrIterationsAdvance)
{
	unsigned int nbr = getNbrOperands ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		Operand *op = getModifiableOperand (i);

		//Paranoid
		if (op != NULL)
		{
			op->updateRegisterName (nbrIterationsAdvance);
		}
	}
}

void Instruction::debug (std::ofstream &out, const Description *desc, int tab) const
{
	std::string s;
	getString (s, desc, tab);
	out << s << std::endl;
}

bool Instruction::isSimilar (const Statement *stmt) const
{
	//Easy comparison
	if (stmt == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is a comment
	const Instruction *inst = dynamic_cast<const Instruction*> (stmt);

	//If not a instruction, we are finished
	if (inst == NULL)
	{
		return false;
	}

	//2) Compare instructions
	if (getOperation ()->isSimilar (inst->getOperation ()) && getCombination () == inst->getCombination () && getSwapBefore () == inst->getSwapBefore () && getSwapAfter () == inst->getSwapAfter ()
			&& getChooseOpAfter () == inst->getChooseOpAfter () && getChooseOpBefore () == inst->getChooseOpBefore ())
	{
		//Now check each operand
		if (getNbrOperands () == inst->getNbrOperands ())
		{
			for (unsigned int i = 0; i < getNbrOperands (); i++)
			{
				const Operand *our = getOperand (i);
				const Operand *their = inst->getOperand (i);

				//If not similar we can get out
				if (our->isSimilar (their) == false)
				{
					return false;
				}
			}

			//All are similar, we are good to go
			return true;
		}
	}

	return false;
}
