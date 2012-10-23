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
 @file ImmediateOperand.cpp
 @brief The ImmediateOperand class is in this file
 */

#include <sstream>

#include "ImmediateOperand.h"

ImmediateOperand::ImmediateOperand (long imm)
{
	init ();
	immediate = imm;
}

ImmediateOperand::ImmediateOperand (long minImm, long maxImm, long progressImm)
{
	init ();
	minImmediate = minImm;
	maxImmediate = maxImm;
	progressImmediate = progressImm;
}

void ImmediateOperand::init (void)
{
	immediate = -1;
	minImmediate = -1;
	maxImmediate = -1;
	progressImmediate = -1;
}

ImmediateOperand::~ImmediateOperand (void)
{
}

long ImmediateOperand::getValImmediate (void) const
{
	return immediate;
}

void ImmediateOperand::setValImmediate (long imm)
{
	immediate = imm;
}

long ImmediateOperand::getMinImmediate (void) const
{
	return minImmediate;
}

void ImmediateOperand::setMinImmediate (long minImm)
{
	minImmediate = minImm;
}

long ImmediateOperand::getMaxImmediate (void) const
{
	return maxImmediate;
}

void ImmediateOperand::setMaxImmediate (long maxImm)
{
	maxImmediate = maxImm;
}

long ImmediateOperand::getProgressImmediate (void) const
{
	return progressImmediate;
}

void ImmediateOperand::setProgressImmediate (long progressImm)
{
	progressImmediate = progressImm;
}

void ImmediateOperand::addString (std::string &s) const
{
	std::ostringstream oss;
	int abs = immediate;

	if (abs < 0)
		abs *= -1;

	oss << "$" << abs;
	s += oss.str ();
}

Operand *ImmediateOperand::copy (void) const
{
	ImmediateOperand *op = new ImmediateOperand (minImmediate, maxImmediate, progressImmediate);
	op->setValImmediate (getValImmediate ());
	return op;
}

bool ImmediateOperand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if op is an immediate operand
	const ImmediateOperand *operand = dynamic_cast<const ImmediateOperand*> (op);

	//If not a memory operand, we are finished
	if (operand == NULL)
	{
		return false;
	}

	//2) Compare immediate memory operands
	if (immediate == operand->getValImmediate () && minImmediate == operand->getMinImmediate () && maxImmediate == operand->getMaxImmediate () && progressImmediate == operand->getProgressImmediate ())
	{
		return true;
	}

	return false;
}

