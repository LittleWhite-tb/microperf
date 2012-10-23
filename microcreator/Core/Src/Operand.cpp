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
 @file Operand.cpp
 @brief The Operand class is in this file
 */

#include "Operand.h"
#include "RegisterOperand.h"

Operand::Operand (void)
{
}

Operand::~Operand (void)
{
}

void Operand::addString (std::string &s) const
{
	(void) s;
}

Operand *Operand::copy (void) const
{
	return new Operand ();
}

void Operand::copyInformation (const Operand *source)
{
	(void) source;
}

void Operand::updateUnroll (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

void Operand::handleInductionVariables (const Kernel *kernel, bool findNewInductions)
{
	(void) kernel;
    (void) findNewInductions;
}

void Operand::updateRegisterName (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

void Operand::fillRegisters (std::vector<RegisterOperand *> &registers)
{
	(void) registers;
}

bool Operand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is an operand
	const Operand *induct = dynamic_cast<const Operand*> (op);

	//If not a instruction, we are finished
	if (induct == NULL)
	{
		return false;
	}

	return false;
}
