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
 @file IndirectMemoryOperand.cpp
 @brief The IndirectMemoryOperand class is in this file
 */

#include <cassert>
#include <sstream>

#include "IndirectMemoryOperand.h"
#include "Kernel.h"
#include "Logging.h"
#include "RegisterOperand.h"

IndirectMemoryOperand::IndirectMemoryOperand (void)
{
	index = NULL;
	multiplier = 0;
}

IndirectMemoryOperand::IndirectMemoryOperand (RegisterOperand *r, RegisterOperand *i, int off, int mult) : MemoryOperand (r, off)
{
	index = NULL;
	setIndexRegister (i);
	setMultiplier (mult);
}

IndirectMemoryOperand::~IndirectMemoryOperand (void)
{
	//Will free memory
	setIndexRegister (NULL);
}

void IndirectMemoryOperand::addString (std::string &s) const
{
	std::ostringstream oss;

	oss << getOffset () << "(";

	if (reg != NULL)
	{
		std::string regString;
		reg->addString (regString);
		oss << regString;
	}
	else
		oss << "NO-BASE-REG";

	if (index != NULL)
	{
		std::string regString;
		index->addString (regString);
		oss << ", " << regString;
	}
	else
		oss << "NO-INDEX-REG";

	oss << ", " << getMultiplier ();
	oss << ")";

	s += oss.str ();
}

Operand *IndirectMemoryOperand::copy (void) const
{
	//Create a copy of the register operand
	RegisterOperand *copy = dynamic_cast<RegisterOperand*> (reg->copy ());
	RegisterOperand *copy_index = dynamic_cast<RegisterOperand*> (index->copy ());

	return new IndirectMemoryOperand (copy, copy_index, getOffset (), getMultiplier ());
}

const RegisterOperand *IndirectMemoryOperand::getBaseRegister (void) const
{
	return reg;
}

RegisterOperand *IndirectMemoryOperand::getModifiableBaseRegister (void)
{
	return reg;
}

const RegisterOperand *IndirectMemoryOperand::getIndexRegister (void) const
{
	return index;
}

RegisterOperand *IndirectMemoryOperand::getModifiableIndexRegister (void)
{
	return index;
}

void IndirectMemoryOperand::handleInductionVariables (const Kernel *kernel,  bool findNewInductions/* = true*/)
{
	//Paranoid, let's check
	if (kernel != NULL)
	{
		if (reg != NULL)
		{
			const std::string &name = reg->getVirtualRegister ();

			const InductionOperand *induction = kernel->getInduction (name);

			if (induction != NULL)
			{
				Logging::log (1, "Warning: Register ", name.c_str (), " is an induction variable and should not be in the IndirectMemoryOperand", NULL);
			}
		}
		else
		{
			Logging::log (1, "Warning: Register is NULL in IndirectMemoryOperand::handleInductionVariables", NULL);
		}

		//Now handle index
		if (index != NULL)
		{
			index->handleInductionVariables (kernel, findNewInductions);
		}
		else
		{
			Logging::log (1, "Warning: Index Register is NULL in IndirectMemoryOperand::handleInductionVariables", NULL);
		}
	}
	else
	{
		Logging::log (1, "Warning: kernel is NULL in IndirectMemoryOperand::handleInductionVariables", NULL);
	}
}

void IndirectMemoryOperand::setIndexRegister (RegisterOperand *r)
{
	delete index, index = NULL;

	index = r;
}

void IndirectMemoryOperand::setBaseRegister (RegisterOperand *r)
{
	setRegister (r);
}

void IndirectMemoryOperand::setMultiplier (int off)
{
	multiplier = off;
}

int IndirectMemoryOperand::getMultiplier (void) const
{
	return multiplier;
}

void IndirectMemoryOperand::fillRegisters (std::vector<RegisterOperand *> &registers)
{
	//Add index
	if (index != NULL)
	{
		index->fillRegisters (registers);
	}

	//Call mother version
	MemoryOperand::fillRegisters (registers);
}

void IndirectMemoryOperand::updateUnroll (int nbrIterationsAdvance)
{
	//Pass information to RegisterOperand
	if (index != NULL)
	{
		offset += index->getOffset () * nbrIterationsAdvance;
	}
}

bool IndirectMemoryOperand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if op is an indirect memory operand
	const IndirectMemoryOperand *operand = dynamic_cast<const IndirectMemoryOperand*> (op);

	//If not an indirec memory operand, we are finished
	if (operand == NULL)
	{
		return false;
	}

	//2) Compare indirect memory operands
	return (operand->getMultiplier () == multiplier && operand->getIndexRegister () == index);
}
