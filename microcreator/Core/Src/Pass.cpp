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
 @file Pass.cpp
 @brief The Pass class is in this file
 */

#include "Pass.h"
#include "PassElement.h"

Pass::Pass (void)
{
	name = "";
}

Pass::~Pass (void)
{
}

const std::string &Pass::getName (void) const
{
	return name;
}

bool Pass::gate (const Description *desc, const Kernel *kernel) const
{
	(void) desc;
	(void) kernel;
	return true;
}

std::vector<PassElement *> *Pass::entry (PassElement *pe, Description *desc) const
{
	(void) pe;
	(void) desc;
	return NULL;
}
