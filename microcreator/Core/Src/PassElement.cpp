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
 @file PassElement.cpp
 @brief The PassElement class is in this file
 */

#include "Kernel.h"
#include "Pass.h"
#include "PassElement.h"

PassElement::PassElement (void)
{
	current = NULL;
	kernel = NULL;
}

PassElement::~PassElement (void)
{
	//It is not our job to delete the pass, we leave that for the PassEngine
	current = NULL;
	delete kernel, kernel = NULL;
}

void PassElement::setPass (Pass *pass)
{
	current = pass;
}

bool PassElement::gate (const Description *desc, const Kernel *kernel) const
{
	if (current == NULL)
		return true;
	return current->gate (desc, kernel);
}

std::vector<PassElement *> *PassElement::entry (Description *desc)
{
	//Reset the current pass
	const Pass *pass = getPass ();

	if (pass == NULL)
		return NULL;

	//Now remove the pass
	setPass (NULL);

	if (pass != NULL)
	{
		return pass->entry (this, desc);
	}
	return NULL;
}

const std::string &PassElement::getPassName (void) const
{
	static std::string empty = "";

	if (current == NULL)
		return empty;

	return current->getName ();
}

const Pass *PassElement::getPass (void) const
{
	return current;
}

Kernel *PassElement::getKernel (void) const
{
	return kernel;
}

void PassElement::setKernel (Kernel *k)
{
	kernel = k;
}
