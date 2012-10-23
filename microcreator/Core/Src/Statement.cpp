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
 @file Statement.cpp
 @brief The Statement class is in this file
 */

#include "Statement.h"

Statement::Statement (void)
{
	minRepeat = 1;
	maxRepeat = 1;
	progressRepeat = 1;
	name = "";
	color = 0;
	linked = "";
	fileNaming = false;
}

Statement::~Statement (void)
{
}

void Statement::setMinRepeat (unsigned int min)
{
	minRepeat = min;
}

unsigned int Statement::getMinRepeat (void) const
{
	return minRepeat;
}

void Statement::setMaxRepeat (unsigned int max)
{
	maxRepeat = max;
}

unsigned int Statement::getMaxRepeat (void) const
{
	return maxRepeat;
}

void Statement::setProgressRepeat (unsigned int progress)
{
	progressRepeat = progress;
}

unsigned int Statement::getProgressRepeat (void) const
{
	return progressRepeat;
}

const std::string &Statement::getName (void) const
{
	return name;
}

void Statement::setName (std::string str)
{
	name = str;
}

const std::string &Statement::getLinked (void) const
{
	return linked;
}

void Statement::setLinked (std::string str)
{
	linked = str;
}

void Statement::setFileNaming (bool fileName)
{
	fileNaming = fileName;
}

bool Statement::getFileNaming (void) const
{
	return fileNaming;
}

void Statement::setTabulations (std::string &s, unsigned int tab) const
{
	s = "";

	for (unsigned int i = 0; i < tab; i++)
	{
		s += "\t";
	}
}

Statement *Statement::findOrigin (const Statement *o)
{
	//Default behavior : if it's us, return us, otherwise NULL
	if (o == origin)
		return this;
	return NULL;
}

void Statement::setColor (unsigned int c)
{
	color = c;
}

unsigned int Statement::getColor (void) const
{
	return color;
}

void Statement::clearColor (void)
{
	color = 0;
}
