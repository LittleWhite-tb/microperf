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
  @file CodeGeneration.cpp
  @brief The CodeGeneration pass is in this file 
 */

#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

#include "CodeGeneration.h"
#include "Description.h"
#include "Instruction.h"
#include "ImmediateOperand.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"
#include "Statement.h"

CodeGeneration::CodeGeneration (void)
{
	name = "Code generation";
}

CodeGeneration::~CodeGeneration (void)
{
}

std::vector <PassElement *> *CodeGeneration::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting Code generation", NULL);

	std::string output = "output";

	//Get kernel
	Kernel *kernel = pe->getKernel ();

	std::ostringstream oss;

    //Get output directory if different
	if (desc != NULL)
	{
		//Get the file name
		oss << desc->getOutputFileName ();

		//Eventually, additional information to the file names
		getKernelInfo (oss, kernel, desc);

		//Now, the extension
		oss << desc->getOutputExtension ();
		output = oss.str ();
	}

	//Try to open the file
	Logging::log (0, "Opening file: ", output.c_str (), NULL);

	std::ofstream out (output.c_str (), std::ios::out);

	if (out.is_open () == false)
	{
		Logging::log (2, "Error: Opening file failed: ", output.c_str (), NULL);
		return NULL;
	}

	//Insert the prologue of C code
	const std::string prologue = desc->getPrologue ();
	if (prologue != "")
	{
		outputFile (out, prologue);
	}

	outputKernel (out, kernel, desc, 0);

	//Insert the prologue of C code
	const std::string epilogue = desc->getEpilogue ();
	if (epilogue != "")
	{
		outputFile (out, epilogue);
	}

	Logging::log (0, "Stopping Code generation", NULL);

	//Close output
	out.close ();

	return NULL;
}

void CodeGeneration::getKernelInfo (std::ostringstream &oss, const Kernel *kernel, const Description *desc) const
{
	//Now we can go through the instructions and actually handle them
	unsigned int nbr = 0;

    //Get number of statements
	if (kernel != NULL)
	{
		nbr = kernel->getNbrStatements ();
	}

    //For each statement
	for (unsigned int i = 0; i < nbr; i++)
	{
		const Statement *stmt = kernel->getStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//Check if statement is a Kernel
		const Kernel *inner = dynamic_cast<const Kernel*> (stmt);

        //Is it a kernel ?
		if (inner != NULL)
		{
            //If so, it might want an extra naming
			if (inner->getFileNaming () == true)
			{
				oss << "_ur" << inner->getActualUnroll ();
			}

            //Recursive
			getKernelInfo (oss, inner, desc);
		}
		else
		{
			const Instruction *inst = dynamic_cast<const Instruction*> (stmt);

            //If it is an instruction
			if (inst !=NULL)
			{
                //Get file naming
				if (inst->getFileNaming () == true)
				{
					unsigned int nbOp = inst->getNbrOperands();

					for (unsigned int j = 0; j < nbOp; j++) 
					{
						//Get operand
						const Operand *op = inst->getOperand (j);

						//We only care about immediate operand
						const ImmediateOperand *immediateOp = dynamic_cast<const ImmediateOperand *> (op);

						if (immediateOp != NULL)
						{
							oss << "_imm" << immediateOp->getValImmediate ();
							return;
						}
					}
				}
			}
		}
	}
}

void CodeGeneration::outputKernel (std::ofstream &out, const Kernel *kernel, const Description *desc, unsigned int tabulation) const
{
	//Now we can go through the instructions and actually handle them
	unsigned int nbr = 0;

    //Paranoid
	if (kernel != NULL)
	{
		nbr = kernel->getNbrStatements ();
	}

    //What is our alignment for the kernel?
	int alignment = 0;
	alignment = kernel->getAlignment ();

	if (alignment != 0)
	{
		for (unsigned int i = 0; i < tabulation; i++)
		{
			out << "\t";
		}

		if (desc->getAsmVolatile () == true)
		{
			out << "asm volatile (\".p2align " << alignment << "\");" << std::endl;
		}
		else
		{
			out << ".p2align " << alignment << std::endl;
		}
	}

    //Do we have a label name for the kernel?
	const std::string &labelName = kernel->getLabelName ();

	if (labelName != "")
	{
		for (unsigned int i = 0; i < tabulation - 1; i++)
		{
			out << "\t";
		}

		if (desc->getAsmVolatile () == true)
		{
			out << std::endl << "asm volatile (\"." << labelName << ":\");" << std::endl << std::endl;
		}
		else
		{
			out << std::endl << "." << labelName << ":" << std::endl << std::endl;
		}
	}

    //Go through each statement now and output it
	for (unsigned int i = 0; i < nbr; i++)
	{
		const Statement *stmt = kernel->getStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//Check if statement is a Kernel
		const Kernel *inner = dynamic_cast<const Kernel*> (stmt);

        //If it is, recursive call
		if (inner != NULL)
		{
			outputKernel (out, inner, desc, tabulation + 1);
		}
		else
		{
			//Now get string for this instruction
			std::string s;

			stmt->getString (s, desc, tabulation);

			//Now write it
			out << s << std::endl;
		}
	}

    //Finally, output the jump if we have one
	const std::string &jump = kernel->getLabelInstruction ();

	if (jump != "")
	{
		for (unsigned int i = 0; i < tabulation; i++)
		{
			out << "\t";
		}

		if (desc->getAsmVolatile () == true)
		{
			out << "asm volatile (\"" << jump << " ." << labelName << "\");" << std::endl;

		}
		else
		{	
			out << jump << " ." << labelName << std::endl;
		}
	}
}

void CodeGeneration::outputFile (std::ofstream &out, const std::string &file) const
{
	std::ifstream in (file.c_str (), std::ios::in);
	std::string buf;

	if (in.is_open () == false)
	{
		Logging::log (1, "Warning: File did not open in CodeGeneration phase: ", file.c_str (), NULL);
	}

	//It did, we can read it and output it
	while (std::getline (in, buf))
	{
		out << buf << std::endl;
	}
}
