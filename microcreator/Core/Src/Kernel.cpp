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
 @file Kernel.cpp
 @brief The Kernel class is in this file
 */

#include <iostream>
#include <cassert>
#include <string.h>

#include "Kernel.h"
#include "InductionOperand.h"
#include "Instruction.h"
#include "Logging.h"
#include "Statement.h"
#include "LoopInfo.h"

Kernel::Kernel (void)
{
	init ();
}

Kernel::~Kernel (void)
{
	clearStatements ();
	clearInductionVariables ();

	sharedVariables.clear ();
	privateVariables.clear ();
}

void Kernel::init (void)
{
	randomize = false;
	combination = true;
	minUnroll = maxUnroll = progressUnroll = actualUnroll = 1;

	labelName = "";
	labelInstruction = "";

	alignment = 0;

	MDLBasicSchedule = false;
	nbBundle = 1;

	minBundle = maxBundle = progressBundle = 1;

	SLoopInfo loopInfo = { "", 1, 1, 1, "", "", { "", 1 } };

	ompOption = false;
	ompParallelFor = false;

	scheduleInfo.type = "";
	scheduleInfo.size = 0;
}

void Kernel::addStatement (const Statement *inst)
{
	Statement *copy = inst->copy ();
	statements.push_back (copy);
}

void Kernel::addStatement (Statement *inst)
{
	statements.push_back (inst);
}

void Kernel::addStatementAt (Statement *inst, unsigned int idx)
{
	statements.insert (statements.begin () + idx, inst);
}

unsigned int Kernel::getNbrStatements (void) const
{
	return statements.size ();
}

const Statement *Kernel::getStatement (unsigned int i) const
{
	if (i >= getNbrStatements ())
		return NULL;

	return statements[i];
}

Statement *Kernel::getModifiableStatement (unsigned int i)
{
	if (i >= getNbrStatements ())
		return NULL;

	return statements[i];
}

void Kernel::replaceStatement (Statement *inst, unsigned int idx, bool free)
{
	//If not valid, we don't do anything
	if (idx >= statements.size ())
		return;

	Statement *old = statements[idx];
	if (free == true)
	{
		delete old, old = NULL;
	}

	statements[idx] = inst;
}
/*
void Kernel::replaceStatement (const Statement *inst, unsigned int idx)
{
	//If not valid, we don't do anything
	if (idx >= statements.size ())
		return;

	Statement *old = statements[idx];
	delete old, old = NULL;
	statements[idx] = inst->copy ();
}*/

bool Kernel::replaceStatement (Statement *inst, Statement *origin)
{
	//Find Statement
	for (std::vector<Statement *>::iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *stmt = *it;

		if (origin == stmt)
		{
			delete stmt, stmt = NULL;
			*it = inst;

			//Found it:
			return true;
		}

		//Special case for internal Kernel
		Kernel *inner = dynamic_cast<Kernel*> (stmt);

		if (inner != NULL)
		{
			//If inner found it, we can stop
			if (inner->replaceStatement (inst, origin) == true)
				return true;
		}
	}

	//Did not find it
	return false;
}

bool Kernel::replaceStatement (const Statement *inst, Statement *origin)
{
	//Find Statement
	for (std::vector<Statement *>::iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *stmt = *it;

		if (origin == stmt)
		{
			delete stmt, stmt = NULL;
			*it = inst->copy ();

			//Found it:
			return true;
		}

		//Special case for internal Kernel
		Kernel *inner = dynamic_cast<Kernel*> (stmt);

		if (inner != NULL)
		{
			//If inner found it, we can stop
			if (inner->replaceStatement (inst, origin))
				return true;
		}
	}

	//Did not find it
	return false;
}

Statement *Kernel::copy (void) const
{
	Kernel *res = new Kernel ();

	res->copyInformation (this);

	//Link it up
	res->origin = this;

	return res;
}

void Kernel::copyInformation (const Statement *stmt)
{
	const Kernel *orig = dynamic_cast<const Kernel*> (stmt);

	//Paranoid
	assert (orig != NULL);

	//Copy superficial
	*this = *orig;

	//Delete vectors : use clear because we don't want to free them
	statements.clear ();

	inductions.clear ();

	//Copy inductions
	unsigned int nbr = orig->getNbrInductions ();

	for (unsigned int i = 0; i < nbr; i++)
	{
		const InductionOperand *induct = orig->getInduction (i);

		InductionOperand *induct_copy = dynamic_cast<InductionOperand*> (induct->copy ());

		addInduction (induct_copy);
	}

	nbr = orig->getNbrStatements ();

	//Now copy the instruction vector
	for (unsigned int i = 0; i < nbr; i++)
	{
		const Statement *st = orig->getStatement (i);

		//Copy the instruction
		Statement *copy = st->copy ();

		//If it is an Instruction
		Instruction *inst = dynamic_cast<Instruction*> (copy);

		if (inst != NULL)
		{
			//Update induction variables
			unsigned int nbr = inst->getNbrOperands ();

			for (unsigned int i = 0; i < nbr; i++)
			{
				Operand *op = inst->getModifiableOperand (i);

				op->handleInductionVariables (orig,false);
			}
		}

		//Add the Statement
		addStatement (copy);
	}
}

void Kernel::append (const Kernel *kernel)
{
	if (kernel != NULL)
	{
		//Now append the vector
		for (std::vector<Statement *>::const_iterator it = kernel->statements.begin (); it != kernel->statements.end (); it++)
		{
			const Statement *stmt = *it;

			//Copy the instruction
			Statement *copy = stmt->copy ();

			//If it is an Instruction
			Instruction *inst = dynamic_cast<Instruction*> (copy);

			if (inst != NULL)
			{
				//Update induction variables
				unsigned int nbr = inst->getNbrOperands ();

				for (unsigned int i = 0; i < nbr; i++)
				{
					Operand *op = inst->getModifiableOperand (i);

					op->handleInductionVariables (this,false);
				}
			}

			//Adding the Statement provokes the copy
			addStatement (copy);
		}
	}
}

void Kernel::handleInductionVariables (void)
{
	//Now append the vector
	for (std::vector<Statement *>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *stmt = *it;

		//If an instruction, we have work
		Instruction *inst = dynamic_cast<Instruction*> (stmt);

		if (inst != NULL)
		{
			//Update induction variables
			unsigned int nbr = inst->getNbrOperands ();

			for (unsigned int i = 0; i < nbr; i++)
			{
				Operand *op = inst->getModifiableOperand (i);

				// Putting false will break the program
				op->handleInductionVariables (this);
			}
		}
		else
		{
			//If a Kernel, we send off recursively
			Kernel *kernel = dynamic_cast<Kernel*> (stmt);

			if (kernel != NULL)
			{
				kernel->handleInductionVariables ();
			}
		}
	}
}

void Kernel::clearStatements (void)
{
	//Delete operand vector
	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		delete tmp, tmp = NULL;
	}
	statements.clear ();
}

void Kernel::clearInductionVariables (void)
{
	//Delete operand vector
	for (std::map<std::string, InductionOperand*>::const_iterator it = inductions.begin (); it != inductions.end (); it++)
	{
		InductionOperand *tmp = it->second;
		delete tmp, tmp = NULL;
	}
	inductions.clear ();
}

void Kernel::copyInductionVariables (const std::map<std::string, InductionOperand*> &inductions_orig)
{
	//Clear original induction variables
	clearInductionVariables ();

	for (std::map<std::string, InductionOperand*>::const_iterator it = inductions_orig.begin (); it != inductions_orig.end (); it++)
	{
		std::string name = it->first;
		InductionOperand *induction = it->second;

		InductionOperand *copy = dynamic_cast<InductionOperand*> (induction->copy ());

		copy->setLinked (NULL);

		inductions[name] = copy;
	}

	//Create a vector with all the InductionOperands
	std::vector<const InductionOperand *> copy;

	//Now we have to handle the links the InductionOperands have
	for (std::map<std::string, InductionOperand*>::const_iterator it = inductions_orig.begin (); it != inductions_orig.end (); it++)
	{
		copy.push_back (it->second);
	}

	//Now handle the linked elements
	handleOurInductionLinks (copy);

	//Free vector
	copy.clear ();
}

InductionOperand *Kernel::getInduction (const std::string &name) const
{	
	//std::cout << "getInduction ( '" << name << ")" << std::endl;
	
	std::map<std::string, InductionOperand *>::const_iterator it = inductions.find (name); //ICI

	if (it != inductions.end ())
		return it->second;
	
	return NULL;
}

void Kernel::updateInductionUnrolling (int iterations)
{
	for (std::map<std::string, InductionOperand*>::const_iterator it = inductions.begin (); it != inductions.end (); it++)
	{
		InductionOperand *induction = it->second;
		induction->updateUnrolling (iterations);
	}
}

unsigned int Kernel::getNbrInductions (void) const
{
	return inductions.size ();
}

const InductionOperand *Kernel::getInduction (unsigned int idx) const
{
	unsigned int cnt = 0;
	std::map<std::string, InductionOperand*>::const_iterator it;

	//If idx is too big
	if (idx >= inductions.size ())
		return NULL;

	for (it = inductions.begin (); it != inductions.end (); it++, cnt++)
	{
		if (cnt == idx)
			break;
	}

	if (cnt == idx)
		return it->second;

	return NULL;
}

InductionOperand *Kernel::getModifiableInduction (unsigned int idx)
{
	unsigned int cnt = 0;
	std::map<std::string, InductionOperand*>::const_iterator it;

	//If idx is too big
	if (idx >= inductions.size ())
		return NULL;

	for (it = inductions.begin (); it != inductions.end (); it++, cnt++)
	{
		if (cnt == idx)
			break;
	}

	if (cnt == idx)
		return it->second;

	return NULL;
}

void Kernel::addInduction (InductionOperand *induction)
{
	if (induction == NULL)
		return;

	const std::string name = induction->getName ();

	//Paranoid
	std::map<std::string, InductionOperand *>::const_iterator it = inductions.find (name);
	assert (it == inductions.end ());

	inductions[name] = induction;
}

void Kernel::addInduction (const std::string &reg, InductionOperand *induction)
{
	if (induction == NULL)
		return;

	//Paranoid
	std::map<std::string, InductionOperand *>::const_iterator it = inductions.find (reg);
	assert (it == inductions.end ());

	inductions[reg] = induction;
}

void Kernel::handleOurInductionLinks (std::vector<const InductionOperand*> &copy)
{
	for (std::vector<const InductionOperand*>::const_iterator it = copy.begin (); it != copy.end (); it++)
	{
		const InductionOperand *induction = *it;

		InductionOperand *linked = induction->getLinked ();

		if (linked != NULL)
		{
			//Get name of this induction
			const std::string name = induction->getName ();

			//Ok now find this induction variable in our map
			std::map<std::string, InductionOperand*>::const_iterator ours = inductions.find (name);

			//Paranoid
			assert (ours != inductions.end ());

			InductionOperand *our_induction = ours->second;

			//Find linked name
			const std::string &linkedName = linked->getName ();

			ours = inductions.find (linkedName);

			//Paranoid
			assert (ours != inductions.end ());

			//Update linked to ours
			linked = ours->second;

			//Set link
			our_induction->setLinked (linked);
		}
	}
}

void Kernel::handleTheirInductionLinks (std::vector<InductionOperand*> &copy)
{
	for (std::vector<InductionOperand*>::const_iterator it = copy.begin (); it != copy.end (); it++)
	{
		InductionOperand *induction = *it;

		//Get name of this induction
		const std::string name = induction->getName ();

		//Now search our map
		std::map<std::string, InductionOperand*>::const_iterator ours = inductions.find (name);

		//Paranoid
		assert (ours != inductions.end ());

		//Get our induction then the linked
		InductionOperand *our_induction = ours->second;
		InductionOperand *linked = our_induction->getLinked ();

		//If there is a link
		if (linked != NULL)
		{
			//Get name of linked
			const std::string linked_name = linked->getName ();

			//Ok now find in our vector the same name
			for (std::vector<InductionOperand*>::const_iterator it_intern = copy.begin (); it_intern != copy.end (); it_intern++)
			{
				InductionOperand *induction_intern = *it_intern;

				//Get name of this induction
				const std::string intern_name = induction_intern->getName ();

				//Check name
				if (intern_name == linked_name)
				{
					induction->setLinked (induction_intern);
					break;
				}
			}
		}
	}
}

void Kernel::replaceInductionVariables (std::vector<InductionOperand*> &copy)
{
	//First clear ours
	clearInductionVariables ();

	for (std::vector<InductionOperand*>::const_iterator it = copy.begin (); it != copy.end (); it++)
	{
		InductionOperand *induction = *it;
		addInduction (induction);
	}
}

bool Kernel::getString (std::string &s, const Description *desc, unsigned int tab) const
{
	(void) s;
	(void) desc;
	(void) tab;
	return false;
}

void Kernel::updateUnrollInformation (int nbrIterationsAdvance)
{
	//Just pass it to the statements
	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		tmp->updateUnrollInformation (nbrIterationsAdvance);
	}
}

void Kernel::updateRegisterName (int nbrIterationsAdvance)
{
	//Just pass it to the statements
	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		tmp->updateUnrollInformation (nbrIterationsAdvance);
	}
}

void Kernel::setRandomize (bool r)
{
	randomize = r;
}

bool Kernel::getRandomize (void) const
{
	return randomize;
}

bool Kernel::getCombination (void) const
{
	return combination;
}

void Kernel::setCombination (bool v)
{
	combination = v;
}

const int &Kernel::getMinUnroll (void) const
{
	return minUnroll;
}

void Kernel::setMinUnroll (int min)
{
	minUnroll = min;
}

const int &Kernel::getMaxUnroll (void) const
{
	return maxUnroll;
}

void Kernel::setMaxUnroll (int max)
{
	maxUnroll = max;
}

const int &Kernel::getProgressUnroll (void) const
{
	return progressUnroll;
}

void Kernel::setProgressUnroll (int progress)
{
	progressUnroll = progress;
}

int Kernel::getActualUnroll (void) const
{
	return actualUnroll;
}

void Kernel::setActualUnroll (int value)
{
	actualUnroll = value;
}

int Kernel::getMinBundle (void) const
{
	return minBundle;
}

void Kernel::setMinBundle (int min)
{
	minBundle = min;
}

int Kernel::getMaxBundle (void) const
{
	return maxBundle;
}

void Kernel::setMaxBundle (int max)
{
	maxBundle = max;
}

int Kernel::getProgressBundle (void) const
{
	return progressBundle;
}

void Kernel::setProgressBundle (int progress)
{
	progressBundle = progress;
}

void Kernel::setLabelName (std::string name)
{
	labelName = name;
}

const std::string &Kernel::getLabelName (void) const
{
	return labelName;
}

void Kernel::setLabelInstruction (const std::string instruction)
{
	labelInstruction = instruction;
}

const std::string &Kernel::getLabelInstruction (void) const
{
	return labelInstruction;
}

int Kernel::getAlignment (void) const
{
	return alignment;
}

void Kernel::setAlignment (int value)
{
	alignment = value;
}

const std::vector<SLoopInfo> &Kernel::getLoopInfo (void) const
{
	return loopInfo;
}

void Kernel::addLoopInfo (SLoopInfo info)
{
	loopInfo.push_back (info);
}

const SScheduleInfo &Kernel::getScheduleInfo (void) const
{
	return scheduleInfo;
}

void Kernel::setScheduleInfo (const SScheduleInfo &info)
{
	scheduleInfo = info;
}

bool Kernel::getMDLBasicSchedule (void) const
{
	return MDLBasicSchedule;
}

void Kernel::setMDLBasicSchedule (bool scheduled)
{
	MDLBasicSchedule = scheduled;
}

int Kernel::getNbBundle (void) const
{
	return nbBundle;
}

void Kernel::setNbBundle (int val)
{
	nbBundle = val;
}

void Kernel::setOmpOption (bool omp)
{
	ompOption = omp;
}

bool Kernel::getOmpOption (void) const
{
	return ompOption;
}

void Kernel::setOmpParallelFor (bool omp)
{
	ompParallelFor = omp;
}

bool Kernel::getOmpParallelFor (void) const
{
	return ompParallelFor;
}
unsigned int Kernel::getNbrSharedVariables (void) const
{
	return sharedVariables.size ();
}

std::string &Kernel::getSharedVariable (unsigned int idx)
{
	return sharedVariables[idx];
}

void Kernel::addSharedVariable (const std::string var)
{
	sharedVariables.push_back (var);
}

unsigned int Kernel::getNbrPrivateVariables (void) const
{
	return privateVariables.size ();
}

std::string &Kernel::getPrivateVariable (unsigned int idx)
{
	return privateVariables[idx];
}

void Kernel::addPrivateVariable (const std::string var)
{
	privateVariables.push_back (var);
}

unsigned int Kernel::getNbrFirstPrivateVariables (void) const
{
	return firstprivateVariables.size ();
}

std::string &Kernel::getFirstPrivateVariable (unsigned int idx)
{
	return firstprivateVariables[idx];
}

void Kernel::addFirstPrivateVariable (const std::string var)
{
	firstprivateVariables.push_back (var);
}

unsigned int Kernel::getNbrLastPrivateVariables (void) const
{
	return lastprivateVariables.size ();
}

std::string &Kernel::getLastPrivateVariable (unsigned int idx)
{
	return lastprivateVariables[idx];
}

void Kernel::addLastPrivateVariable (const std::string var)
{
	lastprivateVariables.push_back (var);
}

unsigned int Kernel::getNbrUsedVariables (void) const
{
	return usedVariables.size ();
}

std::string &Kernel::getUsedVariable (unsigned int idx)
{
	return usedVariables[idx];
}

void Kernel::addUsedVariable (const std::string var)
{
	usedVariables.push_back (var);
}

std::vector<std::string> &Kernel::getListUsedVariables (void)
{
	return usedVariables;
}

void Kernel::debug (std::ofstream &out, const Description *desc, int tab) const
{
	std::string s;
	setTabulations (s, tab);
	out << s;
	out << "Handling kernel (color " << getColor () << ")" << std::endl;

	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		tmp->debug (out, desc, tab + 1);
	}

	out << s;
	out << "Induction variable(s):" << std::endl;
	for (std::map<std::string, InductionOperand*>::const_iterator it_induction = inductions.begin (); it_induction != inductions.end (); it_induction++)
	{
		InductionOperand *tmp = it_induction->second;
		Instruction *inst = tmp->getInstruction ();
		inst->debug (out, desc, tab + 1);
		delete inst, inst = NULL;
	}

	out << s;
	out << "Unroll information: " << getMinUnroll () << " , " << getMaxUnroll () << std::endl;
}

Statement *Kernel::findOrigin (const Statement *o)
{
	if (origin == o)
	{
		return this;
	}

	//Then go in the statements
	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		Statement *res = tmp->findOrigin (o);

		if (res != NULL)
			return res;
	}

	return NULL;
}

void Kernel::clearColor (void)
{
	//Set color to 0
	color = 0;

	//Now handle statements
	for (std::vector<Statement*>::const_iterator it = statements.begin (); it != statements.end (); it++)
	{
		Statement *tmp = *it;
		tmp->clearColor ();
	}
}

bool Kernel::isSimilar (const Statement *stmt) const
{
	//Easy comparison
	if (stmt == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is a comment
	const Kernel *kernel = dynamic_cast<const Kernel*> (stmt);

	//If not a instruction, we are finished
	if (kernel == NULL)
	{
		return false;
	}

	//2) Compare kernels
	if (getRandomize () == kernel->getRandomize () && getMinUnroll () == kernel->getMinUnroll () && getMaxUnroll () == kernel->getMaxUnroll () && getCombination () == kernel->getCombination ()
			&& getProgressUnroll () == kernel->getProgressUnroll () && getLabelName () == kernel->getLabelName () && getLabelInstruction () == kernel->getLabelInstruction ())
	{
		//Now if not the same number of statements
		if (getNbrStatements () != kernel->getNbrStatements ())
		{
			return false;
		}

		//Check each
		for (unsigned int i = 0; i < getNbrStatements (); i++)
		{
			const Statement *our = getStatement (i);
			const Statement *their = kernel->getStatement (i);

			//If not similar we can get out
			if (our->isSimilar (their) == false)
			{
				return false;
			}
		}

		//Now if not the same number of induction
		if (getNbrInductions () != kernel->getNbrInductions ())
		{
			return false;
		}

		//Check each
		for (unsigned int i = 0; i < getNbrInductions (); i++)
		{
			const InductionOperand *our = getInduction (i);
			const InductionOperand *their = kernel->getInduction (i);

			//If not similar we can get out
			if (our->isSimilar (their) == false)
			{
				return false;
			}
		}

		//Finally we are good to go
		return true;
	}

	return false;
}
