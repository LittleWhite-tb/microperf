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
  @file RegisterAllocation.cpp
  @brief The RegisterAllocation pass is in this file 
 */

#include <iostream>
#include <cassert>
#include <fstream>

#include "Description.h"
#include "Instruction.h"
#include "HWInformation.h"
#include "ImmediateOperand.h"
#include "Kernel.h"
#include "Logging.h"
#include "MemoryOperand.h"
#include "Operand.h"
#include "PassElement.h"
#include "RegisterAllocation.h"
#include "RegisterOperand.h"

RegisterAllocation::RegisterAllocation (void)
{
	name = "Register allocation";
}

RegisterAllocation::~RegisterAllocation (void)
{   
}

std::vector <PassElement *> *RegisterAllocation::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Register allocation", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//Do nothing if description doesn't have the hardware information
	if (desc->getHWInformation () != NULL)
	{
		//Get kernel
		Kernel *kernel = pe->getKernel ();

		handleKernel (kernel, desc);
	}
	else
	{
		Logging::log (1, "Warning: Register allocation: Description does not have Hardware information", NULL);
	}

	Logging::log (0, "Stopping Register allocation", NULL);

	//Just re-use the old one
	res->push_back (pe);

	return res;
}


void RegisterAllocation::handleKernel (Kernel *kernel, const Description *desc) const
{
	//Now we can go through the Instruction and actually handle them
	unsigned int nbr = 0;

	if (kernel != NULL)
	{
		nbr = kernel->getNbrStatements ();
	}

	for (unsigned int i = 0; i < nbr; i++)
	{
		Statement *stmt = kernel->getModifiableStatement (i);

		//Send it off
		handleStatement (stmt, desc);
	}
}

void RegisterAllocation::handleStatement (Statement *statement, const Description *desc) const
{
	//Paranoid
	if (statement != NULL)
	{
		//We care if it's an instruction
		Instruction *instruction = dynamic_cast<Instruction*> (statement);

		if (instruction != NULL)
		{
			//Get the operands
			unsigned int nbr = instruction->getNbrOperands ();

			for (unsigned int i = 0; i < nbr; i++)
			{
				Operand *operand = instruction->getModifiableOperand (i);

				handleOperand (operand, desc);
			}
		}

		//We also care if it's a Kernel
		Kernel *kernel = dynamic_cast<Kernel*> (statement);

		if (kernel != NULL)
		{
			handleKernel (kernel, desc);
		}
	}
	else
	{
		Logging::log (1, "Warning: Statement NULL in RegisterAllocation\n");
	}
}

void RegisterAllocation::handleOperand (Operand *operand, const Description *desc) const
{
	//Paranoid
	if (operand != NULL)
	{
		std::vector<RegisterOperand*> registers;

		//Fill the registers
		operand->fillRegisters (registers);

		//For each Operand
		for (std::vector<RegisterOperand*>::const_iterator it = registers.begin (); it != registers.end (); it++)
		{
			RegisterOperand *reg = *it;

			//Paranoid and will break if we have something else like Operand
			assert (reg != NULL);

			//Now handle this one
			handleRegisterOperand (reg, desc);
		}

		registers.clear ();
	}
	else
	{
		Logging::log (1, "Warning: Operand NULL in RegisterAllocation\n");
	}
}

void RegisterAllocation::handleRegisterOperand (RegisterOperand *reg, const Description *desc) const
{
	//Paranoid
	if (reg != NULL)
	{
		//Check if we don't already have the physical name
		if (reg->hasPhysicalRegister () == false)
		{
			const std::string &vname = reg->getName ();

			//Get assocation and update
			const HWInformation *hwInfo = desc->getHWInformation ();

			if (hwInfo != NULL)
			{
				std::string phy = hwInfo->getPhysicalRegister (vname);
				reg->setPhysicalRegister (phy);
			}
		}
	}
	else
	{
		Logging::log (1, "Warning: RegisterOperand NULL in RegisterAllocation\n");
	}
}
