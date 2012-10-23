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
  @file RegisterOperand.cpp
  @brief The RegisterOperand class is in this file 
 */

#include <iostream>
#include <cassert>
#include <sstream>

#include "Kernel.h"
#include "Logging.h"
#include "InductionOperand.h"
#include "RegisterOperand.h"

std::string RegisterOperand::empty = "";

RegisterOperand::RegisterOperand (void)
{
	chosen = 0;
}

RegisterOperand::~RegisterOperand (void)
{
}

void RegisterOperand::addString (std::string &s) const
{
	if (regs.size () > chosen)
	{   
		std::string name = regs[chosen].physicalReg;

		if (name != "")
		{
			s += name;
			return;
		}

		name = regs[chosen].virtualReg;

		//Otherwise, check virtual
		if (name != "")
		{
			s += "(VIRTUAL_REGISTER ";
			s += name;
			s += ")";
			return;
		}
	}

	s += "(None chosen)";
}

Operand *RegisterOperand::copy (void) const
{
	RegisterOperand *copy = new RegisterOperand ();

	copy->copyInformation (this);

	return copy;
}

void RegisterOperand::copyInformation (const RegisterOperand *source)
{
	if (source != NULL)
	{
		*this = *source;

		//Now copy parent
		Operand::copyInformation (source);
	}
}

const std::string &RegisterOperand::getVirtualRegister (unsigned int idx) const
{
	//First check induction variables
	if (regs.size () > idx)
	{
		const InductionOperand *induction = regs[idx].induction;

		//If this one has an induction variable
		if (induction != NULL)
			return induction->getVirtualRegister ();

		return regs[idx].virtualReg;
	}

	//Default behavior
	return empty;
}

const std::string &RegisterOperand::getVirtualRegister (void) const
{
	return getVirtualRegister (chosen);
}

const std::string &RegisterOperand::getPhysicalRegister (unsigned int idx) const
{
	//First check induction variables
	if (regs.size () > idx)
	{
		const InductionOperand *induction = regs[idx].induction;

		//If this one has an induction variable
		if (induction != NULL)
			return induction->getPhysicalRegister ();

		return regs[idx].physicalReg;
	}

	//Default behavior
	return empty;
}

const std::string &RegisterOperand::getPhysicalRegister (void) const
{
	return getPhysicalRegister (chosen);
}

bool RegisterOperand::hasPhysicalRegister (void) const
{
	if (chosen < regs.size ())
	{
		std::string v = regs[chosen].physicalReg;

		return v != "";
	}
	return false;
}

void RegisterOperand::updateRegisterName (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;	

	Logging::log (2,"Error: You shouldn't call the UpdateRegisterName methode in the RegisterOperand", NULL);
	assert (0);
}

int RegisterOperand::getStride (void) const
{
	if (regs.size () > chosen)
	{
		const InductionOperand *induction = regs[chosen].induction;

		if (induction != NULL)
		{
			return induction->getStride ();
		}
	}

	return 0;
}

int RegisterOperand::getOffset (void) const
{
	if (regs.size () > chosen)
	{
		const InductionOperand *induction = regs[chosen].induction;

		if (induction != NULL)
		{
			return induction->getOffset ();
		}
	}

	return 0;
}

void RegisterOperand::handleInductionVariables (const Kernel *kernel, bool findNewInductions)
{
	for (unsigned int i = 0; i < regs.size (); i++)
	{
		if (findNewInductions == true || regs[i].induction != 0)
		{
			const InductionOperand *induction = kernel->getInduction (getName (i));
			
			//std::cout << getName(i) << regs[i].induction << "," << induction << std::endl;
			
			regs[i].induction = induction;
		}
	}
}

void RegisterOperand::fillRegisters (std::vector<RegisterOperand *> &registers)
{
	registers.push_back (this);
}

const std::string &RegisterOperand::getName (int idx) const
{
	//Default behavior, use chosen
	if (idx < 0)
	{
		const std::string &vname = getVirtualRegister ();
		const std::string &pname = getPhysicalRegister ();

		if (pname[0] != '\0')
			return pname;
		return vname;
	}

	//Ok, specific name
	unsigned int uidx = idx;
	if (uidx < regs.size ())
	{
		if (regs[uidx].physicalReg != "")
			return regs[uidx].physicalReg;
		return regs[uidx].virtualReg;
	}

	return empty;
}

void RegisterOperand::pushVirtualName (const std::string &virtualReg)
{
	SRegNames r = {virtualReg, "", NULL};
	regs.push_back (r);
}

void RegisterOperand::pushPhysicalName (const std::string &physicalReg)
{
	SRegNames r = {"", physicalReg, NULL};
	regs.push_back (r);
}

void RegisterOperand::setVirtualRegister (const std::string &virtualReg)
{
	if (regs.size () > chosen)
	{
		regs[chosen].virtualReg = virtualReg;
	}
}

void RegisterOperand::setPhysicalRegister (const std::string &physicalReg)
{
	if (regs.size () > chosen)
	{
		regs[chosen].physicalReg = physicalReg;
	}
}

void RegisterOperand::pushRegisterNames (const std::string &virtualName, const std::string &physicalName)
{
	SRegNames r = {virtualName, physicalName, NULL};
	regs.push_back (r);
}


const std::vector<SRegNames> &RegisterOperand::getRegs (void) const
{
	return regs;
}

unsigned int RegisterOperand::getChosen (void) const
{
	return chosen;
}

bool RegisterOperand::isSimilar (const Operand *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	//Oh well, we have more to do:
	//1) check if op is a RegisterOperand
	const RegisterOperand *operand = dynamic_cast<const RegisterOperand*> (op);

	//If not a memory operand, we are finished
	if (operand == NULL)
	{
		return false;
	}

	//2) Compare register names
	std::vector<SRegNames> their = operand->getRegs ();
	if (regs.size () == their.size ())
	{
		for (std::vector<SRegNames>::const_iterator it = regs.begin (); it != regs.end (); it++)
		{
			SRegNames regName = *it;

			for (std::vector<SRegNames>::const_iterator it_their = their.begin (); it_their != their.end (); it_their++)
			{
				SRegNames regNameTheir = *it_their;

				if (regName.virtualReg != regNameTheir.virtualReg)
				{
					return false;
				}

				if (regName.physicalReg != regNameTheir.physicalReg)
				{
					return false;
				}

				if (!regName.induction->isSimilar(regNameTheir.induction))
				{
					return false;
				}
			}
		}
	}

	return (operand->empty == empty && operand->getChosen () == chosen);
}
