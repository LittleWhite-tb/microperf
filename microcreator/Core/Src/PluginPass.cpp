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
 @file PluginPass.cpp
 @brief The PluginPass class is in this file
 */

#include <iostream>
#include <cassert>

#include "Description.h"
#include "Driver.h"
#include "ImmediateOperand.h"
#include "Instruction.h"
#include "InsertCode.h"
#include "Kernel.h"
#include "Logging.h"
#include "Operand.h"
#include "PluginPass.h"
#include "PassElement.h"
#include "Statement.h"

PluginPass::PluginPass (const SPluginPassInfo &passInfo)
{
	namePtr = passInfo.name;
	gatePluginPass = passInfo.gate;
	entryPtr = passInfo.entry;
}

PluginPass::~PluginPass (void)
{
}

const std::string &PluginPass::getName (void) const
{
	return namePtr ();
}

bool PluginPass::gate (const Description *desc, const Kernel *kernel) const
{
	(void) desc;
	(void) kernel;
	return (gatePluginPass (desc, kernel));
}

std::vector<PassElement*> *PluginPass::entry (PassElement *pe, Description *desc) const
{
	std::vector<PassElement *> *temp = entryPtr (pe, desc);
	return temp;
}

void PluginPass::pluginAddPass (Driver *driver, const SPluginPassInfo &passInfo)
{
	//If the entry pass of the plugin is NULL 
	if (passInfo.entry == NULL)
	{
		std::cerr << "Error: The entry of the plugin is NULL" << std::endl;
		assert (0);
	}

	//If the gate of the plugin pass is NULL 
	if (passInfo.gate == NULL)
	{
		std::cerr << "Error: The gate of the plugin is NULL" << std::endl;
		assert (0);
	}

	//If the plugin pass name is NULL 
	if (passInfo.name == NULL)
	{
		std::cerr << "Error: The name of the plugin is NULL" << std::endl;
		assert (0);
	}

	//If the name of the reference pass of the plugin is NULL 
	if (passInfo.passRef == NULL)
	{
		std::cerr << "Error: The name of reference pass of the plugin is NULL" << std::endl;
		assert (0);
	}

	//Now, launch the plugin
	if (passInfo.pluginPos == PASS_POS_INSERT_AFTER)
	{
		driver->addPassAfter (passInfo.passRef, this);
	}
	else if (passInfo.pluginPos == PASS_POS_INSERT_BEFORE)
	{
		driver->addPassBefore (passInfo.passRef, this);
	}
	else if (passInfo.pluginPos == PASS_POS_REPLACE)
	{
		driver->replacePass (passInfo.passRef, this);
	}
}

