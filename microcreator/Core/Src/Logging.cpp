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
 @file Logging.cpp
 @brief The Logging class is in this file
 */

#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "Logging.h"

//Static variables
Logging *Logging::ourInst = NULL;
bool Logging::firstTime = true;

Logging::Logging (int maxLines)
{
	fileName = "Log.txt"; //output file
	ignore = 0;
	this->maxLines = maxLines;
}

Logging::~Logging (void)
{
	std::cerr << "Log last lines:" << std::endl;

	//Print out vector
	unsigned int size = savedLines.size ();
	unsigned int start = (size >= 10) ? size - 10 : 0;

	for (unsigned int i = start; i < size; i++)
	{
		std::cerr << savedLines[i] << std::endl;
	}
}

void Logging::log (int ignore, ...)
{
	Logging *inst = getInst ();
	va_list vl;

	//Adding the log
	va_start (vl, ignore);
	inst->logIt (ignore, vl);
	va_end (vl);
}

void Logging::logIt (int ignoreLvl, va_list vl)
{
	std::ofstream file;
	std::string s = "";

	//Check if we ignore it
	if (ignoreLvl < ignore)
		return;

	//Creating/Opening log file
	if (firstTime)
	{
		//Creating it
		file.open (fileName.c_str (), std::ofstream::out);
		firstTime = false;
	}
	else
	{
		//Opening it
		file.open (fileName.c_str (), std::ofstream::app);
	}

	//Be paranoid
	if (file.is_open () == false)
	{
		std::cerr << "Error opening file log" << std::endl;
		exit(EXIT_FAILURE);
	}

	//Saving the message
	while (true)
	{
		char *message = va_arg (vl, char*);

		if (message == NULL)
			break;

		s += message;
	}
	file << s << std::endl;
	file.close ();

	//Now add to vector
	savedLines.push_back (s);

	if (savedLines.size () >= maxLines * 2)
	{
		//This means the indices go from 0 to maxLines * 2 - 1
		//So move maxLines -> 0, maxLines + 1 -> 0, maxLines + maxLines - 1 -> maxLines - 1
		for (unsigned int i = 0; i < maxLines; i++)
		{
			savedLines[i] = savedLines[maxLines + i];
		}

		for (unsigned int i = 0; i < maxLines; i++)
		{
			savedLines.pop_back ();
		}
	}
}

void Logging::setLogFile (const std::string &name, bool ft)
{
	Logging *inst = Logging::getInst ();

	inst->fileName = name;
	inst->firstTime = ft;
}

Logging *Logging::getInst (void)
{
	if (ourInst == NULL)
	{
		ourInst = new Logging ();
	}
	return ourInst;
}

void Logging::setIgnore (int value)
{
	Logging *inst = Logging::getInst ();

	inst->ignore = value;
}

void Logging::shutDown (void)
{
	delete ourInst, ourInst = NULL;
}
