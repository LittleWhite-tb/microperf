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
  @file DescriptionXML.cpp
  @brief The DescriptionXML class is in this file 
 */

#include <cassert>
#include <sstream>
#include <fstream>

#include <stdlib.h>
#include <iostream>

#include "DescriptionXML.h"
#include "HWInformation.h"
#include "IndirectMemoryOperand.h"
#include "InductionOperand.h"
#include "Instruction.h"
#include "InsertCode.h"
#include "Kernel.h"
#include "Logging.h"
#include "LoopInfo.h"
#include "MemoryOperand.h"
#include "ImmediateOperand.h"
#include "Operation.h"
#include "RegisterOperand.h"
#include "RegularRegisterOperand.h"
#include "ScheduleInfo.h"

DescriptionXML::DescriptionXML (const std::string &filePath)
{
	Logging::log (0, "Loading parser", NULL);
	init ();

	parse (filePath);

	std::ofstream out ("parseXML_output.out", std::ios::out);

	assert (out.is_open () == true);
	//Debug kernel
	kernel->debug (out, this, 0);

	out.close ();

	Logging::log (0, "Loaded description", NULL);
}

DescriptionXML::~DescriptionXML (void)
{
}

void DescriptionXML::init (void)
{
}

void DescriptionXML::parse (const std::string &filePath)
{
	std::ifstream read (filePath.c_str ());

	bool isEmpty = (read.peek() == EOF);

	if (isEmpty)
	{
		Logging::log (1, "XML: Error: Input file is empty!!", NULL);
		setParsingIsOkay (false);
		return;
	}

	xmlpp::DomParser parser;
	parser.set_substitute_entities();
	parser.parse_file(filePath);

	if (parser)
	{
		//Walk the tree:
		const xmlpp::Node* pNode = parser.get_document()->get_root_node(); //deleted by DomParser.
		parse (pNode);
	}
}

void DescriptionXML::parse (const xmlpp::Node* node)
{
	if(verifyNodeName(node, "description"))
	{
		if (passDescription (node) == false)
		{
			setParsingIsOkay (false);
			return;
		}
	}
	else
	{
		Logging::log (2, "XML: Error: missing description node", NULL);
		setParsingIsOkay (false);
		return;
	}
}

void DescriptionXML::passBranchInformation (const xmlpp::Node* node, Kernel *kernel)
{
	std::string labelName = "", labelInstruction = "";
	xmlpp::Node::NodeList list = node->get_children ();
	std::string label = "", instruction = "" ;

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{

			if (verifyNodeName(tmp, "label"))		
			{
				passLabel (tmp, label);
			}
			else
			{
				if (verifyNodeName(tmp, "test"))		
				{
					passTest (tmp, instruction);
				}
				else
				{
					Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in branch information node at: ", getLine (tmp).c_str (), NULL);
				}
			}
		}
	}

	if (label != "" && instruction != "")
	{
		labelName = label;
		labelInstruction = instruction;
	}
	else
	{
		Logging::log (1, "XML: Warning: Branch Information node error, information missing", NULL);
	}

	kernel->setLabelName (labelName);
	kernel->setLabelInstruction (labelInstruction);
}

bool DescriptionXML::passDescription (const xmlpp::Node* node)
{
	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "kernel"))
			{
				if (passKernel (tmp, kernel) == false)
					return false;
			}
			else if (verifyNodeName(tmp, "benchmark_amount"))
				passBenchmark_amount (tmp);            	
			else if (verifyNodeName (tmp, "hardware_detector"))
				passHardwareDetector (tmp);
			else if (verifyNodeName (tmp, "verbose"))
			{
				verbose = true;
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in description node at: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	return true;
}

std::string DescriptionXML::getLine (const xmlpp::Node *node)
{
	std::ostringstream oss;
	oss << node->get_line ();
	return oss.str ();
}

bool DescriptionXML::passKernel (const xmlpp::Node* node, Kernel *outer)
{
	Kernel *kernel = new Kernel ();

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		assert (tmp != NULL);

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName(tmp, "kernel"))		
				passKernel (tmp, kernel);
			else if (verifyNodeName(tmp, "instruction"))		
			{
				if (passInstruction (tmp, kernel) == false)
                {
                    delete kernel, kernel = NULL;
					return false;
                }
			}
			else if (verifyNodeName (tmp, "name"))
				kernel->setName (extractString (tmp));
			else if (verifyNodeName(tmp, "insert_code"))		
				passInsertCode (tmp, kernel);
			else if (verifyNodeName(tmp, "randomize"))
				kernel->setRandomize (true);
			else if (verifyNodeName(tmp, "unrolling"))
			{
				if (passUnrolling (tmp, kernel) == false)
                {
                    delete kernel, kernel = NULL;
					return false;
                }
			}
			else if (verifyNodeName (tmp, "induction"))
				passInductionOperand (tmp, kernel);
			else if (verifyNodeName (tmp, "repetition"))
				passRepetition (tmp, kernel);
			else if (verifyNodeName (tmp, "branch_information"))
				passBranchInformation (tmp, kernel);
			else if (verifyNodeName (tmp, "alignment"))
				passAlignment (tmp, kernel);
			else if (verifyNodeName (tmp, "loop_info")) 
				passLoopFor (tmp, kernel);
			else
			{	
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in kernel node at: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	//Add kernel to outer kernel
	outer->addStatement (kernel);
	return true;
}

/** Hardware detection */
void DescriptionXML::passHardwareDetector (const xmlpp::Node *node)
{
	//Information we can have
	std::string execution = "";
	std::string hw_file = "";

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		xmlpp::Node *tmp = *iter;

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "execute"))
			{
				execution = extractString (tmp);
			}
			else
			{
				if (verifyNodeName (tmp, "information_file"))
				{
					hw_file = extractString (tmp);
				}
				else
				{
					Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in hardware_detector node: ", getLine (tmp).c_str (), NULL);
				}
			}
		}
	}

	if (hw_file == "")
	{
		Logging::log (2, "XML: Error: missing information_file node in hardware_detector", NULL);
		setParsingIsOkay (false);
		return;
	}

	//Now we can create the HWInformation
	hwInformation = new HWInformation (execution, hw_file);

    //Update our parsing depending on us and the HW information
	setParsingIsOkay (getParsingIsOkay () && hwInformation->getParsingIsOkay ());
}

void DescriptionXML::passInductionOperand (const xmlpp::Node *node, Kernel *kernel)
{
	//Information we can have
	bool affected = true, lastInd = false, noGenerated = false;
	std::string vname = "", pname = "";
	int minStride = 1, maxStride = 1, progressStride = 1;
	int offset = 0, increment = 0;
	InductionOperand *linked = NULL;
	RegisterOperand *inductReg = NULL;
	std::string comment;

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		xmlpp::Node *tmp = *iter;

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "stride"))
			{
				passInductionStride (tmp, minStride, maxStride, progressStride);
			}
			else if (verifyNodeName (tmp, "register"))
			{
				inductReg = passRegister (tmp);                    

				//Paranoid
				assert (inductReg != NULL);
			}
			else if (verifyNodeName (tmp, "not_affected_unroll"))
			{   
				affected = false;
			}
			else if (verifyNodeName (tmp, "last_induction"))
			{
				lastInd = true;
			}
			else if (verifyNodeName (tmp, "offset"))
			{
				std::string s = extractString (tmp);
				offset = convertStringInt (s);
			}
			else if (verifyNodeName (tmp, "increment"))
			{
				std::string s = extractString (tmp);
				increment = convertStringInt (s);
			}
			else if (verifyNodeName (tmp, "comment"))
			{
				std::string s = extractString (tmp);
				comment = s;
			}	
			else if (verifyNodeName (tmp, "linked"))
			{
				linked = passInductionLinked (tmp, kernel);
			}
			else if (verifyNodeName (tmp, "no_generate"))
			{
				noGenerated = true;
			}
			else
			{	
				Logging::log (1, "XML: Warning: Unsupported name  '", extractNodeName (tmp).c_str (), "' in induction node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	//Get names
	vname = inductReg->getVirtualRegister ();
	pname = inductReg->getPhysicalRegister ();

	//Now we can create the Induction variable
	InductionOperand *induction = new InductionOperand (vname, pname, offset, increment, minStride, maxStride, progressStride, affected, lastInd, linked, noGenerated, comment);

	std::string reg = "";

	if (vname[0] != '\0')
		reg = vname;
	else
		reg = pname;

	//Paranoid: is it already there?
	assert (kernel->getInduction (reg) == NULL);

	kernel->addInduction (reg, induction);

	//Free memory
	delete inductReg, inductReg = NULL;	
}

InductionOperand *DescriptionXML::passInductionLinked (const xmlpp::Node* node, Kernel *kernel)
{	
	InductionOperand *res = NULL;
	RegisterOperand *reg = NULL;

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		if (verifyNodeName (tmp, "register"))
			reg = passRegister (tmp);
	}

	//Paranoid
	assert (reg != NULL);

	//Get names
	const std::string vname = reg->getVirtualRegister ();
	const std::string pname = reg->getPhysicalRegister ();

	res = kernel->getInduction (vname);

	if (res == NULL)
	{
		res = kernel->getInduction (pname);
	}

	//Paranoid: we should have found it
	if (res == NULL)
	{
		Logging::log (0, "XML: Error: induction variable: virtual name: ", vname.c_str (), ", physical name: ", pname.c_str (), " not found, is it declared after line: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
	}

	//Free memory
	delete reg, reg = NULL;

	return res;
}

void DescriptionXML::passInductionStride (const xmlpp::Node *node, int &minStride, int &maxStride, int &progressStride)
{
	minMaxProgress (node, minStride, maxStride, progressStride);
}

void DescriptionXML::minMaxProgress (const xmlpp::Node *node, int &min, int &max, int &progress, Statement *curKernel)
{
	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		xmlpp::Node *tmp = *iter;

		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "linked"))
			{
				//Paranoid
				assert (curKernel != NULL);

				curKernel->setLinked (extractString (tmp));				
			}
			else if (verifyNodeName (tmp, "min"))
			{
				std::string s = extractString (tmp);
				min = convertStringInt (s);
			}
			else if (verifyNodeName (tmp, "max"))
			{
				std::string s = extractString (tmp);
				max = convertStringInt (s);
			}
			else if (verifyNodeName (tmp, "progress"))
			{
				std::string s = extractString (tmp);
				progress = convertStringInt (s);
			}
			else if (verifyNodeName (tmp, "file_naming"))
			{
				curKernel->setFileNaming (true);	
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in min/max type node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}
}

/***Begin Insert code part**/
void DescriptionXML::passInsertCode (const xmlpp::Node* node, Kernel *kernel)
{
	std::string inst, file, pro, ep = "";

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		xmlpp::Node *tmp = *iter;
		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "instruction"))
			{
				inst = extractString (tmp);
				InsertCode *insertCode = new InsertCode ("", inst);
				kernel->addStatement (insertCode);
			}
			else if (verifyNodeName (tmp, "file"))
			{
				file = extractString (tmp);
				InsertCode *insertCode = new InsertCode (file, "");
				kernel->addStatement (insertCode);
			}
			else if (verifyNodeName (tmp, "prologue"))
			{
				prologue = extractString (tmp);
				InsertCode *insertCode = new InsertCode ("", "");
				kernel->addStatement (insertCode);
			}
			else if (verifyNodeName (tmp, "epilogue"))
			{
				epilogue = extractString (tmp);
				InsertCode *insertCode = new InsertCode ("", "");
				kernel->addStatement (insertCode);
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' insert_code node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	if ((inst == "") && (file == "") && (prologue == "") && (epilogue == ""))
	{
		Logging::log (0, "XML: Error: unsupported name in insert_code node", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
		return;
	}
}

/***Begin Instruction part**/
bool DescriptionXML::passInstruction (const xmlpp::Node* node, Kernel *kernel)
{	
	//Vector of operations
	std::vector <Operation*> operationVect;
	bool first = true;
	bool swapBefore = false, swapAfter = false;
	bool chooseBefore = false, chooseAfter = false;
	bool immediateBefore = false, immediateAfter = false;
	Instruction* instruction = new Instruction();

	//Recurse through child nodes:
	xmlpp::Node::NodeList list = node->get_children ();
	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp))
		{

			if (verifyNodeName (tmp, "operation"))				
			{
				if (first == true)
				{
					//Add the first operation
					instruction->setOperation (passOperation (tmp));

					first = false;
				}
				else
				{
					operationVect.push_back (passOperation (tmp));
				}
			}
			else if (verifyNodeName (tmp, "register"))
				instruction->addOperand(passRegister (tmp));
			else if (verifyNodeName (tmp, "indirect_memory"))
				instruction->addOperand(passIndirectMemory (tmp));
			else if (verifyNodeName (tmp, "memory"))
				instruction->addOperand(passMemory (tmp));
			else if (verifyNodeName (tmp, "immediate"))
			{
				ImmediateOperand *immOp = NULL;
				immOp = passImmediate (tmp, instruction);

				if (immOp != NULL)
				{
					instruction->addOperand (immOp);
				}
				else
				{
                    delete instruction, instruction = NULL;
					return false;
				}
			}
			else if (verifyNodeName (tmp, "repetition"))
				passRepetition (tmp, instruction);
			else if (verifyNodeName (tmp, "swap_before_unroll"))
			{
				swapBefore = true;
				instruction->setSwapBefore (true);
			}
			else if (verifyNodeName (tmp, "swap_after_unroll"))
			{
				swapAfter = true;
				instruction->setSwapAfter (true);
			}
			else if (verifyNodeName (tmp, "choose_operation_before_unroll"))
			{
				chooseBefore = true;
				instruction->setChooseOpBefore (true);
			}
			else if (verifyNodeName (tmp, "choose_operation_after_unroll"))
			{
				chooseAfter = true;
				instruction->setChooseOpAfter (true);
			}
			else if (verifyNodeName (tmp, "immediate_before_unroll"))
			{
				immediateBefore = true;
				instruction->setImmediateBefore (true);
			}
			else if (verifyNodeName (tmp, "immediate_after_unroll"))
			{
				immediateAfter = true;
				instruction->setImmediateAfter (true);
			}
			else if (verifyNodeName (tmp, "name"))
				instruction->setName (extractString (tmp));
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in instruction node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	if ((swapBefore == true) && (swapAfter == true))
	{
		Logging::log (2, "XML: Error: swap operand is defined before and after unrolling, after line: ", getLine (node).c_str (), NULL);
        delete instruction, instruction = NULL;
		return false;
	}

	if ((chooseBefore == true) && (chooseAfter == true))
	{
		Logging::log (2, "XML: Error: operation_choose is defined before and after unrolling, after line: ", getLine (node).c_str (), NULL);
        delete instruction, instruction = NULL;
		return false;
	}

	if ((immediateBefore == true) && (immediateAfter == true))
	{
		Logging::log (2, "XML: Error: immediate operand is defined before and after unrolling, after line: ", getLine (node).c_str (), NULL);
        delete instruction, instruction = NULL;
		return false;
	}

	//Add the operation vector
	instruction->setOperationVect (operationVect);

	//Add to the kernel
	kernel->addStatement (instruction);

	return true;
}

void DescriptionXML::passLabel (const xmlpp::Node* node, std::string &label)
{
	std::string tmp = extractString (node);

	if (tmp != "")
	{
		label = tmp;
	}
	else
	{
		Logging::log (1, "XML: Warning: missing Label value", NULL);
	}
}

void DescriptionXML::passRepetition (const xmlpp::Node* node, Statement *stmt)
{
	int min = 1, max = 1, progress = 1;

	minMaxProgress (node, min, max, progress);

	if(min > max)
	{
		Logging::log (1, "XML: Error: instruction repetition not enough/wrong parameters at: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
		return;
	}

	//Now set instruction
	stmt->setMinRepeat (min);
	stmt->setMaxRepeat (max);
	stmt->setProgressRepeat (progress);
}

MemoryOperand* DescriptionXML::passMemory (const xmlpp::Node* node)
{
	MemoryOperand* memoryOp = new MemoryOperand ();
	xmlpp::Node::NodeList list = node->get_children ();

	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;
		if (verifyCanGetName (tmp))
		{
			if (verifyNodeName (tmp, "register"))
				memoryOp -> setRegister(passRegister (tmp));
			else if (verifyNodeName (tmp, "offset"))
				passOffset (tmp, memoryOp);
			else
			{
				Logging::log (2, "XML: Error: Unsupported name '", extractNodeName (tmp).c_str (), "' in memory node: ", getLine (tmp).c_str (), NULL);
				setParsingIsOkay (false);
				delete memoryOp, memoryOp = NULL;
				return NULL;
			}	
		}
	}

	return memoryOp ;
}

IndirectMemoryOperand* DescriptionXML::passIndirectMemory (const xmlpp::Node* node)
{
	IndirectMemoryOperand* memoryOp = new IndirectMemoryOperand ();
	xmlpp::Node::NodeList list = node->get_children ();

	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter;

		if (verifyCanGetName (tmp) == true)
		{
			std::string index = "index",
				base = "base";
			if (verifyNodeName (tmp, index))
			{
				memoryOp->setIndexRegister(passRegister (tmp, index));
			}
			else
			{
				if (verifyNodeName (tmp, base))
				{
					memoryOp->setBaseRegister(passRegister (tmp, base));
				}
				else
				{
					if (verifyNodeName (tmp, "offset"))
						passOffset (tmp, memoryOp);
					else
					{
						if (verifyNodeName (tmp, "multiplier"))
						{
							std::string tmp2 = extractString (tmp);
							memoryOp->setMultiplier (convertStringInt (tmp2));
						}
						else
						{
							Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in indirect_memory node: ", getLine (tmp).c_str (), NULL);
						}
					}
				}
			}
		}
	}

	if (memoryOp->getIndexRegister () == NULL)
	{
		Logging::log (2, "Error: index register in indirect memory operand is NULL", NULL);
		setParsingIsOkay (false);
		
		//Free it 
		delete memoryOp, memoryOp = NULL;

		return NULL;	
	}

	if (memoryOp->getBaseRegister () == NULL)
	{
		Logging::log (2, "Error: base register in indirect memory operand is NULL", NULL);
		setParsingIsOkay (false);

		//Free it 
		delete memoryOp, memoryOp = NULL;

		return NULL;
	}

	return memoryOp;
}

Operation* DescriptionXML::passOperation (const xmlpp::Node* node)
{
    std::string str = extractString (node);
	return new Operation (OP_TYPE_UNKNOWN, str);
}

RegisterOperand* DescriptionXML::passRegister (const xmlpp::Node* node, const std::string &nodeName)
{
	(void) nodeName;

	xmlpp::Node::NodeList list = node->get_children ();

	int max = -1, min = -1;
	std::string name = "";
	std::string vname = "";

	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "name"))
				vname =  extractString (tmp);
			else if (verifyNodeName (tmp, "phyName"))
				name =  extractString (tmp);
			else if (verifyNodeName (tmp, "min"))
				min = convertStringInt (extractString (tmp));
			else if (verifyNodeName (tmp, "max"))
				max = convertStringInt (extractString (tmp));
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in register node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	if ((name == "") && (vname == ""))
	{
		Logging::log (2, "XML: Error: Node in register operand hasn't a name node at line: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
	}

	RegularRegisterOperand* reg = new RegularRegisterOperand (vname, name, min, max);
	return reg;

	return NULL;
}

void DescriptionXML::passOffset (const xmlpp::Node* node, MemoryOperand* memoryOperand)
{
	std::string tmp = extractString (node);
	memoryOperand->setOffset (convertStringInt (tmp));
}

ImmediateOperand* DescriptionXML::passImmediate (const xmlpp::Node* node, Instruction *curInst)
{
	ImmediateOperand* immediatOp = NULL;
	long value  = -1, min = -1, max = -1, progressImm = 1; 
	std::string slink = "";
	bool fileNaming = false;

	xmlpp::Node::NodeList list = node->get_children ();

	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "value")) 											
				value = convertStringInt (extractString (tmp));
			else if (verifyNodeName (tmp, "min")) 				
				min = convertStringInt (extractString (tmp)); 
			else if (verifyNodeName (tmp, "max"))
				max = convertStringInt (extractString (tmp));
			else if (verifyNodeName (tmp, "progress"))
				progressImm = convertStringInt (extractString (tmp));					
			else if (verifyNodeName (tmp, "linked"))
				slink = extractString (tmp);		
			else if (verifyNodeName (tmp, "file_naming"))
				fileNaming = true;
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in immediate node: ", getLine (tmp).c_str (), NULL);
			}
		}	
	}

	if (progressImm < 0)
	{
		Logging::log (2, "XML: Error: negative value at line: ", getLine (node).c_str (),NULL);
		setParsingIsOkay (false);
		return NULL;
	}

	if (min > max)
	{
		Logging::log (2, "XML: Error: Wrong parameters of min/max of the immediate operand at: ", getLine (node).c_str (),NULL);
		setParsingIsOkay (false);
		return NULL;
	}

	if (((min != -1 || max != -1) && (value != -1)) || (min > max))
	{
		Logging::log (1, "XML: Warning: immediate value : wrong parameters ",NULL);
	}
	else
	{
		if (value != -1)
		{
			immediatOp = new ImmediateOperand (value);
		}
		else	
		{
			immediatOp = new ImmediateOperand (min, max, progressImm);				
		}

		//Update the link
		curInst->setLinked (slink);

		//File names with additional information
		curInst->setFileNaming (fileNaming);
	}

	return immediatOp; 			
}

void DescriptionXML::passBenchmark_amount (const xmlpp::Node* node)
{
	std::string tmp = extractString (node);
	maxBenchmarks = convertStringInt (tmp);		
}

bool DescriptionXML::passUnrolling (const xmlpp::Node* node, Kernel *kernel)
{
	int minUnroll = 1, maxUnroll = 1, progressUnroll = 1;

	minMaxProgress (node, minUnroll, maxUnroll, progressUnroll, kernel);

	if ((minUnroll < 0) || (maxUnroll < 0) || (progressUnroll < 0))
	{
		Logging::log (2, "XML: Error: negative value, after line: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
		return false;
	}

	if (minUnroll > maxUnroll)
	{
		Logging::log (2, "XML: Warning: wrong parameters of for unrolling at: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
		return false;
	}

	//Now set kernel
	kernel->setMinUnroll (minUnroll);
	kernel->setMaxUnroll (maxUnroll);
	kernel->setProgressUnroll (progressUnroll);

	return true;
}

void DescriptionXML::passTest (const xmlpp::Node* node, std::string &instruction)
{
	std::string tmp = extractString(node);

	if (tmp != "")
	{
		instruction = tmp;
	}
	else
	{
		Logging::log (0, "XML: missing instruction of the label", NULL);
	}
}

void DescriptionXML::passAlignment (const xmlpp::Node* node, Kernel *kernel)
{
	unsigned int val = 0;

	val = getAlignmentValue (convertStringInt (extractString(node)));

	kernel->setAlignment (val);
}

int DescriptionXML::getAlignmentValue (int val)
{
	int result = 0;
	while (val != 0)
	{
		val /= 2;
		result++;
	}
	return (result - 1);
}

void DescriptionXML::passLoopFor (const xmlpp::Node* node, Kernel *kernel)
{
	c_code = true;

	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "count_iteration"))
			{
				passCountIteration (tmp, kernel);			
			}
			else if (verifyNodeName (tmp, "induction"))
			{
				passInductionOperand (tmp, kernel);
			}
			else if (verifyNodeName (tmp, "omp_option"))
			{
				passOmpOption (tmp, kernel);
			}
			else if (verifyNodeName (tmp, "schedule"))
			{
				passScheduleInfo (tmp, kernel);
			}
			else if (verifyNodeName (tmp, "pragma_omp_parallel"))
			{
				kernel->setOmpParallelFor (true);	
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in loop_info node at: ", getLine (tmp).c_str (), NULL);
			}   
		}
	}
}

void DescriptionXML::passCountIteration (const xmlpp::Node* node, Kernel *kernel)
{
	SLoopInfo info = {"", 1, 1, 1, "", "", {"", 0}};

	std::string minStr = "", maxStr = "";

	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "induction"))
			{
				info.induction = extractString (tmp);
			}				
			else if (verifyNodeName (tmp, "increment"))
			{
				info.increment = convertStringInt (extractString (tmp));
			}
			else if (verifyNodeName (tmp, "min"))
			{
				info.min = convertStringInt (extractString (tmp));
			}
			else if (verifyNodeName (tmp, "max"))
			{
				info.max = convertStringInt (extractString (tmp));
			}
			else if (verifyNodeName (tmp, "minVariable"))
			{
				info.minStr = extractString (tmp);
			}
			else if (verifyNodeName (tmp, "maxVariable"))
			{
				info.maxStr = extractString (tmp);
			}
			else if (verifyNodeName (tmp, "register"))
			{
				info.reg = passRegisterFor (tmp);
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in count_iteration node at: ", getLine (tmp).c_str (), NULL);
			}   
		}
	}
	
	kernel->addLoopInfo (info);
}

void DescriptionXML::passScheduleInfo (const xmlpp::Node* node, Kernel *kernel)
{
	SScheduleInfo info = {"", 0};

	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "type"))
			{
				info.type = extractString (tmp);
			}				
			else if (verifyNodeName (tmp, "size"))
			{
				info.size = convertStringInt (extractString (tmp));
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in schedule node at: ", getLine (tmp).c_str (), NULL);
			}   
		}
	}
	
	kernel->setScheduleInfo (info);
}

SRegName DescriptionXML::passRegisterFor (const xmlpp::Node* node)
{
	xmlpp::Node::NodeList list = node->get_children ();

	std::string name = "";
	unsigned int offset = 0;

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "name"))
				name =  extractString (tmp);
			else if (verifyNodeName (tmp, "offset"))
				offset = convertStringInt (extractString (tmp));
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in register node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	if (name == "")
	{
		Logging::log (2, "XML: Error: Node in register operand hasn't a name node at line: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
	}

	SRegName reg = {name, offset};

	return reg;
}

void DescriptionXML::passOmpOption (const xmlpp::Node* node, Kernel *kernel)
{
	kernel->setOmpOption (true);

	xmlpp::Node::NodeList list = node->get_children ();

	for (xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "private"))
			{
				passOmpOptionRegister (tmp, "private", kernel);
			}				
			else if (verifyNodeName (tmp, "shared"))
			{
				passOmpOptionRegister (tmp, "shared", kernel);
			}
			else if (verifyNodeName (tmp, "firstprivate"))
			{
				passOmpOptionRegister (tmp, "firstprivate", kernel);
			}
			else if (verifyNodeName (tmp, "lastprivate"))
			{
				passOmpOptionRegister (tmp, "lastprivate", kernel);
			}
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in omp_option node at: ", getLine (tmp).c_str (), NULL);
			}  
		}
	}
}

void DescriptionXML::passOmpOptionRegister (const xmlpp::Node* node, const std::string &nodeName, Kernel *kernel)
{
	xmlpp::Node::NodeList list = node->get_children ();

	int max = -1, min = -1;
	std::string name = "";

	for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		const xmlpp::Node *tmp = *iter ;

		if (verifyCanGetName (tmp) == true)
		{
			if (verifyNodeName (tmp, "name"))
				name =  extractString (tmp);
			else if (verifyNodeName (tmp, "min"))
				min = convertStringInt (extractString (tmp));
			else if (verifyNodeName (tmp, "max"))
				max = convertStringInt (extractString (tmp));
			else
			{
				Logging::log (1, "XML: Warning: Unsupported name '", extractNodeName (tmp).c_str (), "' in omp_option node: ", getLine (tmp).c_str (), NULL);
			}
		}
	}

	if (name == "")
	{
		Logging::log (2, "XML: Error: Node in omp_option hasn't a name node at line: ", getLine (node).c_str (), NULL);
		setParsingIsOkay (false);
	}

	if (min > -1) 
	{	
		if (min < max)
		{
			for (int i = min; i <= max; i++)
			{
				std::ostringstream oss;
				oss << name << i;										

				if (nodeName == "private")
				{
					kernel->addPrivateVariable (oss.str ().c_str ());
				}				
				else if (nodeName == "shared")
				{
					kernel->addSharedVariable (oss.str ().c_str ());
				}
				else if (nodeName == "firstprivate")
				{
					kernel->addFirstPrivateVariable (oss.str ().c_str ());
				}
				else if (nodeName == "lastprivate")
				{
					kernel->addLastPrivateVariable (oss.str ().c_str ());
				}		
			}
		}
		else
		{
			Logging::log (2, "XML: Error: Wrong parameter (min/max value) in omp_option node at line: ", getLine (node).c_str (), NULL);
			setParsingIsOkay (false);
		}
	}
	else
	{
		if (nodeName == "private")
		{
			kernel->addPrivateVariable (name);
		}				
		else if (nodeName == "shared")
		{
			kernel->addSharedVariable (name);
		}
		else if (nodeName == "firstprivate")
		{
			kernel->addFirstPrivateVariable (name);
		}
		else if (nodeName == "lastprivate")
		{
			kernel->addLastPrivateVariable (name);
		}		
	}
}

