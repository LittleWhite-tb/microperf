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
  @file RegularRegisterOperand.cpp
  @brief The RegularRegisterOperand class is in this file 
 */

#include <cassert>
#include <sstream>

#include "RegularRegisterOperand.h"

RegularRegisterOperand::RegularRegisterOperand (const std::string &RegOpVirtualName, const std::string &RegOpPhysicalName, int min, int max, bool order, int cur)
{
	//Paranoid: can't have a vname and a physical name straight away
	assert (! (RegOpVirtualName != "" && RegOpPhysicalName != ""));

	//Check whether we are RegOpVirtualName or RegOpPhysicalName
	if (RegOpVirtualName != "")
	{
		fillRegisterNames (RegOpVirtualName, true, min, max);
	}
	else
	{
		fillRegisterNames (RegOpPhysicalName, false, min, max);
	}

	//Paranoid: we should have at least one element
	assert (regs.size () > 0);

	if (cur == -1)
		chosen = 0;
	else
		chosen = cur % regs.size ();

	inOrder = order;
}

RegularRegisterOperand::~RegularRegisterOperand (void)
{

}

void RegularRegisterOperand::fillRegisterNames (const std::string &name, bool isVirtual, int min, int max)
{
	for (int i = min; i <= max; i++)
	{
		//Create name
		std::string s = name;

		//Now add i if != -1
		if (i != -1)
		{
			std::ostringstream oss;
			oss << i;
			s += oss.str ();
		}

		//Push names
		if (isVirtual)
		{
			pushRegisterNames (s, "");
		}
		else
		{
			pushRegisterNames ("", s);
		}
	}
}

void RegularRegisterOperand::updateRegisterName (int nbrIterationsAdvance)
{
	assert (regs.size () > 0);

	int tmp = nbrIterationsAdvance % regs.size ();

	chosen = tmp;
}

Operand *RegularRegisterOperand::copy (void) const
{
	//We don't need to do anything for the arguments, because RegisterOperand will copy our names for us
	RegularRegisterOperand *reg = new RegularRegisterOperand ("", "", 0, 0, inOrder, 0);

	reg->RegisterOperand::copyInformation (this);

	return reg;
}

bool RegularRegisterOperand::getInOrder (void) const
{
	return inOrder;
}

bool RegularRegisterOperand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is an regular register operand
	const RegularRegisterOperand *regularRegOp = dynamic_cast<const RegularRegisterOperand*> (op);

	//If not a instruction, we are finished
	if (regularRegOp == NULL)
	{
		return false;
	}

	return (getInOrder () == regularRegOp->getInOrder ());
}

