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
  @file OMPCode.cpp
  @brief The OMPCode pass is in this file 
 */

#include <iostream>
#include <cassert>
#include <fstream>

#include "Comment.h"
#include "Description.h"
#include "InductionOperand.h"
#include "InsertCode.h"
#include "Instruction.h"
#include "HWInformation.h"
#include "ImmediateOperand.h"
#include "Kernel.h"
#include "Logging.h"
#include "LoopInfo.h"
#include "MemoryOperand.h"
#include "Operand.h"
#include "Operation.h"
#include "OMPCode.h"
#include "PassElement.h"
#include "RegisterOperand.h"

#include <algorithm>

OMPCode::OMPCode (void)
{
	name = "omp code";
}

OMPCode::~OMPCode (void)
{   
}

std::vector <PassElement *> *OMPCode::entry (PassElement *pe, Description *desc) const
{
	Logging::log (0, "Starting omp code generation", NULL);

	//Prepare result
	std::vector <PassElement *> *res = new std::vector <PassElement *> ();

	//Get kernel
	Kernel *kernel = pe->getKernel ();

	//Vectors of register declaration
	std::set <std::string> regOperand, memOperand;

	handleKernel (kernel, desc, regOperand, memOperand);

	//Create a copy
	Kernel *copy = dynamic_cast<Kernel*> (kernel->copy ());

	//Paranoid
	assert (copy != NULL);

	//Now remove instructions
	kernel->clearStatements ();

	//Set the c extension to the output
	desc->setOutputExtension (".c");

	//Adding a C comment
	InsertCode *comment = new InsertCode ("", "\t//Registers declaration");
	kernel->addStatement (comment);

	//Correspondance between register variable and register name
	std::map <std::string, std::string> declarationRegisters;

	//Adding register declaration
	addRegisterDeclaration (kernel, regOperand, declarationRegisters);

	//Adding memory register declaration
	addRegisterDeclaration (kernel, memOperand, declarationRegisters, true);

	//Assign the array to a variable (momentarily)
	InsertCode *assignment = new InsertCode ("", "\tr0 = x;");
	kernel->addStatement (assignment);

	//Adding the omp parallel for
	handleOmpOption (copy, kernel);

	std::vector<SLoopInfo> infoVect;

	//the unroll factor
	unsigned int unrollFactor;

	//Adding the loop for expression 
	handleLoopHeader (copy, kernel, infoVect, unrollFactor);

	//Handling the body of the loop
	handleInstruction (copy, declarationRegisters, infoVect);

	//Add the return statement
	handleLoopReturn (copy, infoVect, unrollFactor);

	kernel->append (copy);

	//Now delete the copy
	delete copy, copy = NULL;		

	Logging::log (0, "Stopping omp code generation", NULL);

	//Just re-use the old one
	res->push_back (pe);

	return res;
}

void OMPCode::addRegisterDeclaration (Kernel *kernel, std::set <std::string> registers, std::map <std::string, std::string> &declarationRegisters, bool pointer) const
{
	unsigned int i = 0;

    //For each register
	for (std::set<std::string>::const_iterator it = registers.begin (); it != registers.end (); it++)
	{
		std::ostringstream oss, ossInst, ossTmp;

		//Get register name
		const std::string regName = *it;

		int idx = regName.find_last_of ('%');

		//Go after 
		idx++;

		std::string str = *it;

		oss << i;

		ossInst << i;

		//Fill the map
		declarationRegisters[regName] = regName.substr (idx, 1) + ossInst.str () ;

		std::string st = "";

		if (pointer == true)
		{
			st = "\tregister float *";
		}
		else
		{
			st = "\tregister float ";
		}

		int closed = 0;
		closed = regName.find_last_of (')');
		int pos = closed - idx;

		//Add the register variable to the list of the used variables
		std::string usedVar =  regName.substr (idx, 1) + oss.str ();
		kernel->addUsedVariable (usedVar);

		//Create the declaration expression
		const std::string regDeclaration = st + regName.substr (idx, 1) + oss.str () + " asm (\"" + regName.substr (idx - 1, pos + 1) + "\");";
		InsertCode *decl = new InsertCode ("", regDeclaration, true);

		//Add the declaration
		kernel->addStatement (decl);

		i++;		
	}
}

const std::string &OMPCode::getRegisterDeclaration (const std::string &name, std::map <std::string, std::string> &reg) const
{
	static const std::string res = "";

	std::map<std::string, std::string>::const_iterator it;

	//Find it
	it = reg.find (name);

	//Did we find it ?
	if (it != reg.end ())
	{
		return it->second;
	}

	return res;
}

void OMPCode::handleOmpOption (Kernel *kernel, Kernel *origin) const
{
	unsigned int nbr = kernel->getNbrStatements ();

	bool ompOptionDefined = false;

	for (unsigned int i = 0; i < nbr; i++)
	{
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//Just to be sure that this kernel contains instructions
		Instruction *inst = dynamic_cast<Instruction *> (stmt);

		if (inst != NULL)
		{
			if (ompOptionDefined == false)	
			{
				if (kernel->getOmpOption () == true)
				{
					std::ostringstream oss;
					if (kernel->getOmpParallelFor () == true)
					{
						oss << "\t#pragma omp parallel for ";

						//add the option just if we have `#pragma omp parallel for`
						addOptions ("firstprivate", kernel, origin, oss);
						addOptions ("lastprivate", kernel, origin, oss);
						addOptions ("shared", kernel, origin, oss);
						addOptions ("private", kernel, origin, oss);
					}
					else  
					{
						oss << "\t#pragma omp for ";

						//add the information about the schedule
						const SScheduleInfo info_tmp = kernel->getScheduleInfo ();

                        //If we have something
                        if (info_tmp.type != "" && info_tmp.size != 0)
                        {
                            oss << "schedule (" << info_tmp.type;

                            if (info_tmp.size == 0)
                            {
                                oss << ")";
                            }
                            else
                            {
                                oss << ", " << kernel->getScheduleInfo ().size << ")";
                            }
                        }
					}
			
					//Finally, insert the option
					InsertCode *omp = new InsertCode ("", oss.str ());
					origin->addStatement (omp);
				}

				ompOptionDefined = true;
			}
		}
		else
		{
			//Check if statement is a Kernel
			Kernel *inner = dynamic_cast<Kernel*> (stmt);

			if (inner != NULL)
			{
				handleOmpOption (inner, origin);
			}
		}
	}
}

void OMPCode::addOptions (const std::string option, Kernel *kernel, Kernel *origin, std::ostringstream &oss) const
{
	std::vector <std::string> firstprivateVect, lastprivateVect, sharedVect, privateVect;
	unsigned int nbr = 0; 

	//Add variables	
	if (option == "firstprivate")
	{
		nbr = kernel->getNbrFirstPrivateVariables ();
	}
	else if (option == "lastprivate")
	{
		nbr = kernel->getNbrLastPrivateVariables ();
	}
	else if (option == "shared")
	{
		nbr = kernel->getNbrSharedVariables ();
	}
	else if (option == "private")
	{
		nbr = kernel->getNbrPrivateVariables ();
	}
		
	if (nbr > 0)
	{
		//Check if the registers of the option are already declared, remove them if they don't exist
		for (unsigned int i = 0; i < nbr; i++)
		{
			std::string st = "";
			
			if (option == "firstprivate")
			{
				st = kernel->getFirstPrivateVariable (i);
				
				std::vector <std::string> listUsedVar = origin->getListUsedVariables ();

				bool contains = false;

				//If it exists in the list of declared registers
				contains = std::find (listUsedVar.begin (), listUsedVar.end (), st) != listUsedVar.end ();

				//Remove registers not declared
				if (contains == true)
				{
					//kernel->removeFirstPrivateVariable (i);
					firstprivateVect.push_back (st);
				}
			}
			else if (option == "lastprivate")
			{
				st = kernel->getLastPrivateVariable (i);

				std::vector <std::string> listUsedVar = origin->getListUsedVariables ();

				bool contains = false;

				//If it exists in the list of declared registers
				contains = std::find (listUsedVar.begin (), listUsedVar.end (), st) != listUsedVar.end ();

				//Remove registers not declared
				if (contains == true)
				{
					lastprivateVect.push_back (st);
				}
			}
			else if (option == "shared")
			{
				st = kernel->getSharedVariable (i);

				std::vector <std::string> listUsedVar = origin->getListUsedVariables ();

				bool contains = false;

				//If it exists in the list of declared registers
				contains = std::find (listUsedVar.begin (), listUsedVar.end (), st) != listUsedVar.end ();

				//Remove registers not declared
				if (contains == true)
				{
					sharedVect.push_back (st);
				}

			}
			else if (option == "private")
			{
				st = kernel->getPrivateVariable (i);

				std::vector <std::string> listUsedVar = origin->getListUsedVariables ();

				bool contains = false;

				//If it exists in the list of declared registers
				contains = std::find (listUsedVar.begin (), listUsedVar.end (), st) != listUsedVar.end ();

				//Remove registers not declared
				if (contains == true)
				{
					privateVect.push_back (st);
				}
			}
		}

		//add the options 
		if (option == "firstprivate")
		{
			nbr = firstprivateVect.size ();
			if (nbr > 0)
			{
				oss << "firstprivate (";
	
				for (unsigned int i = 0; i < nbr; i++)
				{
					std::string st = firstprivateVect [i];

					oss << st;

					if (i == nbr - 1)
					{
						oss << ") ";	
					}
					else
					{
						oss << ", ";
					}
				}
			}	
		}
		else if (option == "lastprivate")
		{
			nbr = lastprivateVect.size ();
			if (nbr > 0)
			{
				oss << "lastprivate (";
	
				for (unsigned int i = 0; i < nbr; i++)
				{
					std::string st = lastprivateVect [i];

					oss << st;

					if (i == nbr - 1)
					{
						oss << ") ";	
					}
					else
					{
						oss << ", ";
					}
				}
			}	
		}
		else if (option == "shared")
		{
			nbr = sharedVect.size ();

			if (nbr > 0)
			{
				oss << "shared (";
	
				for (unsigned int i = 0; i < nbr; i++)
				{
					std::string st = sharedVect [i];

					oss << st;

					if (i == nbr - 1)
					{
						oss << ") ";	
					}
					else
					{
						oss << ", ";
					}
				}
			}	
		}
		else if (option == "private")
		{
			nbr = privateVect.size ();

			if (nbr > 0)
			{
				oss << "private (";
	
				for (unsigned int i = 0; i < nbr; i++)
				{
					std::string st = privateVect [i];

					oss << st;

					if (i == nbr - 1)
					{
						oss << ") ";	
					}
					else
					{
						oss << ", ";
					}
				}
			}	
		}
	}
}

void OMPCode::handleLoopHeader (Kernel *kernel, Kernel *origin, std::vector<SLoopInfo> &infoVect, unsigned int &unrollFactor) const
{
	unsigned int nbr = kernel->getNbrStatements ();

	bool loopHeaderDefined = false;

	for (unsigned int i = 0; i < nbr; i++)
	{
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//Just to be sure that this kernel contains instructions
		Instruction *inst = dynamic_cast<Instruction *> (stmt);

		if (inst != NULL)
		{
			if (loopHeaderDefined == false)	
			{
				infoVect = kernel->getLoopInfo ();
				unrollFactor = kernel->getActualUnroll ();
				std::ostringstream oss;

				for (unsigned int j = 0; j < infoVect.size (); j++)
				{
					if (j == 0)
					{
						oss << "\tfor (" << infoVect[0].induction << " = ";
						
						std::string minStr = "";

						minStr = infoVect[0].minStr;

						if (minStr == "")
						{
							oss << infoVect[0].min;
						}
						else
						{
							oss << minStr.c_str ();
						}
					}
					else
					{
						oss << ", " << infoVect[j].induction << " = ";
			
						std::string minStr = "";
						minStr = infoVect[j].minStr;

						if (minStr == "")
						{
							oss << infoVect[j].min;
						}
						else
						{
							oss << minStr.c_str ();
						}
					}
				}
				
				oss << "; ";

				for (unsigned int j = 0; j < infoVect.size (); j++)
				{
					if (j == 0)
					{
						std::string maxStr = "";
						maxStr = infoVect[j].maxStr;

						oss << infoVect[j].induction << " < ";

						if (maxStr == "")
						{
							oss << infoVect[j].max;
						}	
						else
						{
							oss << maxStr.c_str ();
						}
					}
					else
					{
						oss << " && ";
						std::string minStr = "", maxStr = "";
						maxStr = infoVect[j].maxStr;
						
						oss << infoVect[j].induction << " < ";

						if (maxStr == "")
						{
							oss << infoVect[j].max;
						}	
						else
						{
							oss << maxStr.c_str ();
						}
					}
				}
		
				oss << "; ";

				for (unsigned int j = 0; j < infoVect.size (); j++)
				{
					if (j == 0)
					{
						oss << infoVect[0].induction << " += (" << unrollFactor << " * " << infoVect[0].increment << ")";
					}
					else 
					{
						oss << ", " << infoVect[j].induction << " += (" << unrollFactor << " * " << infoVect[j].increment << ")";
					}
				}
			
				oss << ")";

				InsertCode *headerLoop = new InsertCode ("", oss.str ());
				origin->addStatement (headerLoop);

				//Add opened accolade
				InsertCode *openedAccolade = new InsertCode ("", "\t{");
				origin->addStatement (openedAccolade);

				loopHeaderDefined = true;
			}
		}
		else
		{
			//Check if statement is a Kernel
			Kernel *inner = dynamic_cast<Kernel*> (stmt);

			if (inner != NULL)
			{
				handleLoopHeader (inner, origin, infoVect, unrollFactor);
			}
		}
	}
}

void OMPCode::handleLoopReturn (Kernel *kernel, std::vector<SLoopInfo> &infoVect, unsigned int &unrollFactor) const
{
	//Add closed accolade
	InsertCode *closedAccolade = new InsertCode ("", "\t}");
	kernel->addStatement (closedAccolade);

	if (infoVect.size () == 1)
	{
		std::string minStr = "", maxStr = "";
		int min = 0, max = 0;

		maxStr = infoVect[0].maxStr;

		if (minStr == "")
		{
			min = infoVect[0].min;
		}

		if (maxStr == "")
		{
			max = infoVect[0].max;
		}

		std::ostringstream oss;

		//Add the loop expression
		if (maxStr != "")
		{
			oss << "\treturn ((" << maxStr;
		}	
		else
		{
			oss << "\treturn ((" << max;
		}

		if (minStr != "")
		{
			oss << " - " << minStr << ") / (" << unrollFactor << " * " << infoVect[0].increment << "));";
		}	
		else
		{
			oss << " - " << min << ") / (" << unrollFactor << " * " << infoVect[0].increment << "));";
		}

		InsertCode *returnStmt = new InsertCode ("", oss.str ());
		kernel->addStatement (returnStmt);
	}
	else
	{
		std::ostringstream oss;
		oss << "\t//Compute number of iterations \n";
		oss << "\tint _tab[" << (infoVect.size ()) << "] = {";
	
		//Compute the minimum of (max-main)/unroll of all the vector
		//Start by filling the array
		for (unsigned int i = 0; i < infoVect.size (); i++)
		{
			std::string minStr = "", maxStr = "";
			int min = 0, max = 0;

			maxStr = infoVect[i].maxStr;
			minStr = infoVect[i].minStr;

			if (minStr == "")
			{
				min = infoVect[i].min;
			}

			if (maxStr == "")
			{
				max = infoVect[i].max;
			}

			if (i != 0)
			{
				oss << ", ";
			}

			//Add the loop expression
			if (maxStr != "")
			{
				oss << "(" << maxStr;
			}		
			else
			{
				oss << "(" << max;
			}	

			if (minStr != "")
			{
				oss << " - " << minStr << ") / (" << unrollFactor << " * " << infoVect[i].increment << ")";
			}	
			else
			{
				oss << " - " << min << ") / (" << unrollFactor << " * " << infoVect[i].increment << ")";
			}
		
			if (maxStr != "")
			{
				oss << " + (" << maxStr << " > ";
			}		
			else
			{
				oss << " + (" << max << " > ";
			}	

			if (minStr != "")
			{
				oss << minStr << ")";
			}		
			else
			{
				oss << min << ")";
			}	
		}

		oss << "};\n";
		oss << "\tint i = 0; \n";
		oss << "\tint min = _tab[0];\n";
		oss << "\tfor (i = 0; i < " << infoVect.size () << "; " << "i++)\n";
		oss << "\t{\n";
		oss << "\t\tif (_tab[i] < min) \n";
		oss << "\t\t{\n";
		oss << "\t\t\tmin = _tab[i];\n";
		oss << "\t\t}\n";
		oss << "\t}\n";
		oss << "\treturn min;";
	
		InsertCode *returnStmt = new InsertCode ("", oss.str ());
		kernel->addStatement (returnStmt);
	}
}

void OMPCode::handleInstruction (Kernel *kernel, std::map <std::string, std::string> &declRegs, std::vector<SLoopInfo> &infoVect, unsigned int start) const
{
	unsigned int nbr = kernel->getNbrStatements ();
	unsigned int count = 0;

	for (unsigned int i = start; i < nbr; i++)
	{
		Statement *stmt = kernel->getModifiableStatement (i);

		//Paranoid
		assert (stmt != NULL);

		//We only care about instructions
		Instruction *inst = dynamic_cast<Instruction *> (stmt);

		if (inst != NULL)
		{
			//Get operation
			Operation *operation = inst->getModifiableOperation ();

			if (operation != NULL)
			{
				std::string s;

				//Now write it
				s += "\t__asm__ __volatile__ (\"";

				std::string operationName = "";
				operation->addString (operationName);

				//Add operation name
				s += operationName;

				//Add the options	
				s += " %1, %0\" : ";

				//Now for each operand, add it
				std::string src = "", dest = "";

				Operand *op0 = inst->getModifiableOperand (0);
				Operand *op1 = inst->getModifiableOperand (1);

				RegisterOperand *reg0 = dynamic_cast<RegisterOperand*> (op0);
				RegisterOperand *reg1 = dynamic_cast<RegisterOperand*> (op1);
				MemoryOperand *mem0 = dynamic_cast<MemoryOperand*> (op0);
				MemoryOperand *mem1 = dynamic_cast<MemoryOperand*> (op1);
				ImmediateOperand *imm0 = dynamic_cast<ImmediateOperand*> (op0);
				ImmediateOperand *imm1 = dynamic_cast<ImmediateOperand*> (op1);

				//Destination register name
				if (reg1 != NULL)
				{
					//Get correspondance	
					dest = getRegisterDeclaration (reg1->getName (), declRegs);

					std::string st = "\"=x\" (" + dest + ")";
					s += st;
				}
				else if (mem1 != NULL)
				{
					std::ostringstream oss;
					dest = mem1->getModifiableRegister()->getName();

					//Get correspondance
					dest = getRegisterDeclaration (dest, declRegs);
				
					for (unsigned int j = 0; j < infoVect.size (); j++)
					{
						SRegName reg = infoVect[j].reg;

						if (reg.name == dest)
						{
							oss << dest << " + (" << infoVect[j].induction.c_str () << " + " << (count * reg.offset) << ")";
							if (infoVect.size () == 1)
								count += (j+1);
							else
								count += j;

							break;
						}
					}
					dest = oss.str ();
					std::string st = "\"=m\" (*(" + dest + "))";
					s += st;
				}
				else if (imm1 != NULL)
				{
					imm1->addString (dest);
					std::string st = "\"=i\" (" + dest + ")";
					s += st;
				}

				//Add separator
				s += " : ";

				//Source register name
				if (reg0 != NULL)
				{
					//Get correspondance
					src = getRegisterDeclaration (reg0->getName (), declRegs);

					//@todo: handle each type of register
					std::string st = "\"x\" (" + src + ")";
					s += st;
				}
				else if (mem0 != NULL)
				{
					std::ostringstream oss;
					src = mem0->getModifiableRegister()->getName();

					//Get correspondance
					src = getRegisterDeclaration (src, declRegs);
				
					for (unsigned int j = 0; j < infoVect.size (); j++)
					{
						SRegName reg = infoVect[j].reg;

						if (reg.name == src)
						{
							oss << src << " + (" << infoVect[j].induction.c_str () << " + " << (count * infoVect[j].reg.offset) << ")";
							if (infoVect.size () == 1)
								count += (j+1);
							else
								count += j;

							break;
						}
					}

					src = oss.str ();
					std::string st = "\"m\" (*(" + src + "))";
					s += st;
				}
				else if (imm0 != NULL)
				{	
					imm0->addString (src);
					std::string st = "\"i\" (\"" + src + "\")";
					s += st;
				}

				//Closing the inlining
				s += ");";

				InsertCode *inlineAsm = new InsertCode ("", s);

				//Replace instruction with my new instruction
				kernel->replaceStatement (inlineAsm, i);
			}
		}
		else
		{
			//Check if is a comment
			Comment *comment = dynamic_cast<Comment*> (stmt);

			if (comment != NULL)
			{
				std::string s;

				//Now write it
				s += "\t__asm__ __volatile__ (\"#";

				comment->addString (s);

				//Closing the inlining
				s += "\");";

				InsertCode *inlineAsm = new InsertCode ("", s);

				//Replace with the new comment
				kernel->replaceStatement (inlineAsm, i);
			}
			else
			{	
				//Check if statement is a Kernel
				Kernel *inner = dynamic_cast<Kernel*> (stmt);

				if (inner != NULL)
				{
					handleInstruction (inner, declRegs, infoVect, start);
				}
			}
		}
	}
}

void OMPCode::handleKernel (Kernel *kernel, const Description *desc, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const
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
		handleStatement (stmt, desc, regOperand, memOperand);
	}
}

void OMPCode::handleStatement (Statement *statement, const Description *desc, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const
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

				handleOperand (operand, regOperand, memOperand);
			}
		}

		//We also care if it's a Kernel
		Kernel *kernel = dynamic_cast<Kernel*> (statement);

		if (kernel != NULL)
		{
			handleKernel (kernel, desc, regOperand, memOperand);
		}
	}
	else
	{
		Logging::log (1, "Warning: Statement NULL in OMPCode\n");
	}
}

void OMPCode::handleOperand (Operand *operand, std::set <std::string> &regOperand, std::set <std::string> &memOperand) const
{
	//Paranoid
	if (operand != NULL)
	{
		RegisterOperand *regOp = dynamic_cast<RegisterOperand*> (operand);
		MemoryOperand *memOp = dynamic_cast<MemoryOperand*> (operand);

		std::string s = "";
		if (regOp != NULL)
		{
			regOp->addString (s);		
			regOperand.insert (s);
		}
		else if (memOp != NULL)
		{
			//memOp->addString (s);
			s = memOp->getRegister ()->getName ();
			memOperand.insert (s);
		}
	}
	else
	{
		Logging::log (1, "Warning: Operand NULL in OMPCode\n");
	}
}
