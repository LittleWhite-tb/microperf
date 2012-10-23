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
 @file InsertCode.cpp
 @brief The InsertCode class is in this file
 */

#include <iostream>
#include <cassert>
#include <fstream>

#include "Description.h"
#include "InsertCode.h"
#include "Logging.h"

InsertCode::InsertCode (void)
{
	init ();
}

InsertCode::InsertCode (const std::string &comment, const std::string &instr, bool insertTab)
{
	init();
	fileName = comment;
	insertTabs = insertTab;
	instruction = instr;
}

InsertCode::~InsertCode (void)
{

}

void InsertCode::init ()
{
	fileName = "";
	instruction = "";
	insertTabs = false;
}

const std::string InsertCode::getFileName (void) const
{
	return fileName;
}

void InsertCode::setFileName (const std::string &name)
{
	fileName = name;
}

const std::string InsertCode::getInstruction (void) const
{
	return instruction;
}

void InsertCode::setInstruction (const std::string &name)
{
	instruction = name;
}

bool InsertCode::getString (std::string &s, const Description *desc, unsigned int tab) const
{
	std::string st = "";

	if (desc->getAsmVolatile () == true)
	{
		setTabulations (s, tab);
		s += "asm volatile (\"";
	}

	if (fileName != st)
	{
		std::string tmp;
		std::ifstream in (fileName.c_str (), std::ios::in);

		//Paranoid
		if (in.is_open () == false)
		{
			Logging::log (2, "Error: Could not find file: ", fileName.c_str (), " for InsertCode", NULL);
			assert (0);
		}

		//Read the lines
		while (std::getline (in, tmp))
		{
			std::string tmp2 = "";

			if (insertTabs == true)
			{
				setTabulations (tmp2, tab);
			}

			tmp2 += tmp;

			//Now add to output string
			s += tmp2 + "\n";
		}
	}
	else if (instruction != st)
	{
		std::string tmp = instruction;
		setTabulations (tmp, tab);
		tmp += instruction;
		s += tmp;
	}

	if (desc->getAsmVolatile () == true)
	{
		s += "\");";
	}

	(void) desc;
	return true;
}

Statement* InsertCode::copy (void) const
{
	InsertCode *comment = new InsertCode ();

	comment->copyInformation (this);

	//Link it
	comment->origin = this;

	return comment;
}

void InsertCode::copyInformation (const Statement *stmt)
{
	const InsertCode *orig = dynamic_cast<const InsertCode*> (stmt);

	if (orig != NULL)
	{
		setFileName (orig->getFileName ());
		setInstruction (orig->getInstruction ());
	}
}

void InsertCode::debug (std::ofstream &outStream, const Description *desc, int tab) const
{
	std::string s;
	getString (s, desc, tab);
	outStream << s << std::endl;
}

void InsertCode::updateUnrollInformation (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

void InsertCode::updateRegisterName (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

bool InsertCode::isSimilar (const Statement *stmt) const
{
	//Easy comparison
	if (stmt == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is a comment
	const InsertCode *c = dynamic_cast<const InsertCode*> (stmt);

	//If not a comment, we are finished
	if (c == NULL)
	{
		return false;
	}

	//2) Compare comments
	return (fileName == c->fileName && instruction == c->instruction && insertTabs == c->insertTabs);
}
