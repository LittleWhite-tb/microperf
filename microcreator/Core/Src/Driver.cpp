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
 @file Driver.cpp
 @brief The Driver class is in this file
 */

#include <cassert>
#include <iostream>

#include "Description.h"
#include "Driver.h"
#include "PassEngine.h"
#include "Logging.h"

//The different passes
#include "CodeGeneration.h"
#include "ImmediateSelection.h"
#include "InductionInsertion.h"
#include "InductionSelection.h"
#include "Kernel.h"
#include "StrideSelection.h"
#include "StatementSelection.h"
#include "OMPCode.h"
#include "OperandSwap.h"
#include "OperationChoose.h"
#include "RegisterAllocation.h"
#include "PassUnroll.h"
#include "PluginPass.h"

Driver::Driver (void)
{
	init ();
}

Driver::Driver (Description *desc)
{
	init ();
	description = desc;
}

Driver::~Driver (void)
{
	delete passEngine, passEngine = 0;
	delete description, description = 0;
}

void Driver::init (void)
{
	description = 0;
	passEngine = 0;
}

PassEngine *Driver::getPassEngine (void)
{
	return passEngine;
}

bool Driver::addPass (Pass *newPass)
{
	if (passEngine == 0)
		return false;

	return passEngine->addPass (newPass);
}

bool Driver::addPassAfter (const std::string &after, Pass *newPass)
{
	if (passEngine == 0)
		return false;

	return passEngine->addPassAfter (after, newPass);
}

bool Driver::replacePass (const std::string &name, Pass *newPass)
{
	if (passEngine == 0)
		return false;

	return passEngine->replacePass (name, newPass);
}

bool Driver::addPassBefore (const std::string &before, Pass *newPass)
{
	if (passEngine == 0)
		return false;

	return passEngine->addPassBefore (before, newPass);
}

bool Driver::removePass (const std::string &name)
{
	if (passEngine == 0)
		return false;

	return passEngine->removePass (name);
}

bool Driver::setPluginGate (const SPluginGateInfo &gateInfo)
{
	if (passEngine == 0)
		return false;

	return passEngine->setPluginGate (gateInfo);
}

void Driver::drive (void)
{
	Logging::log (0, "Driving", 0);

	if (passEngine == 0)
	{
		Logging::log (1, "Warning: There is no Pass Engine", 0);
	}
	else
	{
		passEngine->drivePasses (description);
	}

	Logging::log (0, "Drove", 0);
}

void Driver::addGeneralPasses (void)
{
	Logging::log (0, "Adding passes to Driver", 0);

	//Create pass engine
	passEngine = new PassEngine ();

	//Add passes here
	StatementSelection *stmtSched = new StatementSelection ();
	addPass (stmtSched);

	InductionSelection *indSched = new InductionSelection ();
	addPass (indSched);

	//Immediate Selection before unroll
	ImmediateSelection *im = new ImmediateSelection (true);
	addPass (im);

	//Operand swap before unroll
	OperandSwap *os = new OperandSwap (true);
	addPass (os);

	//Operation choose before unroll
	OperationChoose *oc = new OperationChoose (true);
	addPass (oc);

	//Stride selection
	StrideSelection *ss = new StrideSelection ();
	addPass (ss);

	//Unroll    
	PassUnroll *ur = new PassUnroll ();
	addPass (ur);

	//Operation choose after unroll
	oc = new OperationChoose (false);
	addPass (oc);

	//Operand swap after unroll
	os = new OperandSwap (false);
	addPass (os);

	//Immediate Selection after unroll
	im = new ImmediateSelection (false);
	addPass (im);

	InductionInsertion *is = new InductionInsertion ();
	addPass (is);

	RegisterAllocation *ra = new RegisterAllocation ();
	addPass (ra);

	//Generation of C code containing eventually omp pragmas
	if ((description != 0) && (description->getC_code () == true))
	{
		OMPCode *omp = new OMPCode ();
		addPass (omp);
	}

	CodeGeneration *cg = new CodeGeneration ();
	addPass (cg);

	//Stop adding passes here
	Logging::log (0, "Added passes to Driver", 0);
}
