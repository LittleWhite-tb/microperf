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
  @file InductionOperand.cpp
  @brief The InductionOperand class is in this file 
 */

#include <cassert>

#include "ImmediateOperand.h"
#include "InductionOperand.h"
#include "Instruction.h"
#include "Operation.h"
#include "Logging.h"

InductionOperand::InductionOperand (const std::string &RegOpVirtualName, const std::string &RegOpPhysicalName, int offset,
		int increment, int minStride, int maxStride, int progressStride, bool unroll, bool last, InductionOperand *linked, bool noGenerated,
		const std::string comment)
{
	Logging::log(0, "Creating InductionOperand variable", NULL);

	init ();

	//If we have no increment, it becomes the offset
	if (increment == 0)
	{
		increment = offset;
	}

	setOffset (offset);
	setIncrement (increment);
	setMinimumStride (minStride);
	setMaximumStride (maxStride);
	setProgressStride (progressStride);

	setAffected (unroll);
	setLastInduction(last);

	//Push names
	pushRegisterNames (RegOpVirtualName, RegOpPhysicalName);

	setLinked (linked);
	setNoGenerated (noGenerated);

	setComment (comment);

	Logging::log(0, "End of InductionOperand variable creation", NULL);
}

InductionOperand::~InductionOperand (void)
{

}

void InductionOperand::init (void)
{
	stride = 1;
	lastInduction = false;
	noGenerated = false;

	//Set unroll
	unroll = 1;
}

int InductionOperand::getStride (void) const
{
	if (linked != NULL)
		return linked->getStride ();

	return stride;
}

void InductionOperand::setStride (int value)
{
	stride = value;
}

InductionOperand* InductionOperand::getLinked (void) const
{
	return linked;
}

void InductionOperand::setLinked (InductionOperand* value)
{
	linked = value;
}

int InductionOperand::getIncrement (void) const
{
	if (linked)
		return increment * (linked->getStride ());

	if (getMinimumStride () == getStride ())
	{	
		return increment;
	}
	else	
	{
		return increment * getStride ();
	}
}

int InductionOperand::getBaseIncrement (void) const
{
	if (linked)
		return linked->getBaseIncrement ();

	return increment;
}

void InductionOperand::setIncrement (int value)
{
	increment = value;
}

int InductionOperand::getOffset (void) const
{
	if (linked != NULL)
		return offset * linked->getStride ();

	return offset * getStride ();
}

int InductionOperand::getBaseOffset (void) const
{
	if (linked != NULL)
		return linked->getBaseOffset ();

	return getStride ();
}

void InductionOperand::setOffset (int value)
{
	offset = value;
}

int InductionOperand::getUnroll (void) const
{
	return unroll;
}

void InductionOperand::setUnroll (int value)
{
	unroll = value;
}

int InductionOperand::getMaximumStride (void) const
{
	return maxStride;
}

void InductionOperand::setMaximumStride (int value)
{
	maxStride = value;
}

int InductionOperand::getMinimumStride (void) const
{
	return minStride;
}

void InductionOperand::setMinimumStride (int value)
{
	minStride = value;
}

int InductionOperand::getProgressStride (void) const
{
	return progressStride;
}

void InductionOperand::setProgressStride (int value)
{
	progressStride = value;
}

Operand *InductionOperand::copy (void) const
{
	InductionOperand *copy = new InductionOperand (getVirtualRegister (), getPhysicalRegister (), getOffset (), getIncrement (), 
			getMinimumStride (), getMaximumStride (), getProgressStride (), getAffected (), getLastInduction (),
			NULL, getNoGenerated (), getComment ());

	//Call parent copyInformation
	RegisterOperand *r_copy = static_cast<RegisterOperand *> (copy);
	r_copy->copyInformation (this);

	return copy;
}

void InductionOperand::setAffected (bool b)
{
	affectedUnroll = b;
}

bool InductionOperand::getAffected (void) const
{
	return affectedUnroll;
}

void InductionOperand::setComment (const std::string comment)
{
	this->comment = comment;
}

const std::string InductionOperand::getComment (void) const
{
	return comment;
}

Instruction *InductionOperand::getInstruction (void) const
{
	Instruction *res = new Instruction ();
	int inc = getIncrement ();

	if (inc < 0)
	{
		std::string name = "sub";
		Operation *op = new Operation (OP_TYPE_ADD, name);
		res->setOperation (op);
	}
	else
	{
		std::string name = "add";
		Operation *op = new Operation (OP_TYPE_ADD, name);
		res->setOperation (op);
	}

	//Add immediate
	ImmediateOperand *imm = new ImmediateOperand (inc);
	res->addOperand (imm);

	//Now add register
	Operand *operand = copy ();
	res->addOperand (operand);

	res->setComment (comment);

	return res;
}

void InductionOperand::updateUnrolling (int iterations) 
{
	if (getAffected ())
	{
		//If linked, we have nothing to do
		if (linked != NULL)
			return;

		//Get stride
		int stride = getStride ();

		//Update stride
		stride *= (iterations);

		//Now set it
		setStride (stride);
	}
}

void InductionOperand::setLastInduction (bool lastInd)
{
	lastInduction = lastInd;
}

int InductionOperand::getLastInduction (void) const
{
	return lastInduction;
}

int InductionOperand::getOrigStride (void) const
{
	return origStride;
}

void InductionOperand::setNoGenerated (bool val)
{
	noGenerated = val;
}

int InductionOperand::getNoGenerated (void) const
{
	return noGenerated;
}

bool InductionOperand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is an InductionOperand
	const InductionOperand *induct = dynamic_cast<const InductionOperand*> (op);

	//If not a instruction, we are finished
	if (induct == NULL)
	{
		return false;
	}

	//2) Compare induction operands
	if (linked->isSimilar (induct->getLinked ()) &&
			getStride () == induct->getStride () &&
			getIncrement () == induct->getIncrement () &&
			getAffected () == induct->getAffected () &&
			getLastInduction () == induct->getLastInduction () &&
			getOffset () == induct->getOffset () &&
			getUnroll () == induct->getUnroll() &&
			getMaximumStride () == induct->getMaximumStride() &&
			getMinimumStride () == induct->getMinimumStride() &&
			getProgressStride () == induct->getProgressStride () &&
			getOrigStride () == induct->getOrigStride ())
	{
		//All are similar, we are good to go
		return true;
	}

	return false;
}

