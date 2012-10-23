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
 @file MemoryOperand.cpp
 @brief The MemoryOperand class is in this file
 */

#include <cassert>
#include <sstream>

#include "Kernel.h"
#include "Logging.h"
#include "MemoryOperand.h"
#include "RegisterOperand.h"

MemoryOperand::MemoryOperand (void)
{
	reg = NULL;
	offset = 0;
}

MemoryOperand::MemoryOperand (RegisterOperand *regOp, int offset)
{
	reg = NULL;
	setRegister (regOp);
	setOffset (offset);
}

MemoryOperand::~MemoryOperand (void)
{
	//Will free memory
	setRegister (NULL);
}

void MemoryOperand::addString (std::string &s) const
{
	std::ostringstream oss;

	oss << offset << "(";

	//Paranoid
	if (reg != NULL)
	{
		std::string regString;
		reg->addString (regString);
		oss << regString;
	}
	else
		oss << "NO-REG";

	oss << ")";
	s += oss.str ();
}

Operand *MemoryOperand::copy (void) const
{
	//Create a copy of the register operand
	RegisterOperand *copy = dynamic_cast<RegisterOperand*> (reg->copy ());

	return new MemoryOperand (copy, offset);
}

void MemoryOperand::setOffset (int off)
{
	offset = off;
}

int MemoryOperand::getOffset (void) const
{
	return offset;
}

void MemoryOperand::setRegister (RegisterOperand *regOp)
{
	delete reg, reg = NULL;

	reg = regOp;
}

const RegisterOperand *MemoryOperand::getRegister (void) const
{
	return reg;
}

RegisterOperand *MemoryOperand::getModifiableRegister (void)
{
	return reg;
}

void MemoryOperand::updateUnroll (int nbrIterationsAdvance)
{
	//Pass information to RegisterOperand
	if (reg != NULL)
	{
		offset += reg->getOffset () * nbrIterationsAdvance;
	}
}

void MemoryOperand::updateRegisterName (int nbrIterationsAdvance)
{
	if (reg != NULL)
	{
		reg->updateRegisterName (nbrIterationsAdvance);
	}
}

void MemoryOperand::handleInductionVariables (const Kernel *kernel, bool findNewInductions/* = true*/)
{
	if (kernel != NULL)
	{
		if (reg != NULL)
		{
			reg->handleInductionVariables (kernel, findNewInductions);
		}
		else
		{
			Logging::log (1, "Warning: Register is NULL in MemoryOperand::handleInductionVariables", NULL);
		}
	}
	else
	{
		Logging::log (1, "Warning: kernel is NULL in MemoryOperand::handleInductionVariables", NULL);
	}
}

void MemoryOperand::fillRegisters (std::vector<RegisterOperand *> &registers)
{
	if (reg != NULL)
	{
		reg->fillRegisters (registers);
	}
}

bool MemoryOperand::isSimilar (const Operand *op) const
{
	bool registerSimilar = true;

	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if op is a MemoryOperand
	const MemoryOperand *operand = dynamic_cast<const MemoryOperand*> (op);

	//If not a memory operand, we are finished
	if (operand == NULL)
	{
		return false;
	}

	const RegisterOperand *reg = getRegister ();

	if (reg != NULL)
	{
		const RegisterOperand *reg2 = operand->getRegister ();

		if (reg2 != NULL)
		{
			registerSimilar = reg->isSimilar (reg2);
		}
		else
		{
			registerSimilar = false;
		}
	}

	//2) Compare memory operands
	return (operand->getOffset () == getOffset () && registerSimilar);
}
