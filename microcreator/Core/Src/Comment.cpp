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
 @file Comment.cpp
 @brief The Comment class is in this file
 */

#include <cassert>

#include "Description.h"
#include "Comment.h"

Comment::Comment (void)
{
	init ();
}

Comment::Comment (const std::string &com)
{
	init ();
	comment = com;
}

Comment::~Comment (void)
{

}

void Comment::init ()
{
	comment = "";
}

void Comment::addString (std::string &s)
{
	s += comment;
}

void Comment::setComment (const std::string &s)
{
	comment = s;
}

const std::string Comment::getComment (void) const
{
	return comment;
}

bool Comment::getString (std::string &s, const Description *desc,
		unsigned int tab) const
{
	setTabulations (s, tab);

	//Add asm volatile to the comments
	if (desc->getAsmVolatile () == true)
	{
		s += "asm volatile (\"#";
		s += comment;
		s += "\");";
	}
	else
	{
		s += "#";
		s += comment;
	}

	(void) desc;
	return true;
}

void Comment::clearComment (void)
{
	comment = "";
}

Statement* Comment::copy (void) const
{
	Comment *com = new Comment ();

	//Link it up
	com->origin = this;
	com->copyInformation (this);

	return com;
}

void Comment::copyInformation (const Statement *stmt)
{
	const Comment *orig = dynamic_cast<const Comment*> (stmt);

	//Paranoid
	assert (orig != NULL);

	setComment (orig->getComment ());
}

void Comment::debug (std::ofstream &out, const Description *desc, int tab) const
{
	std::string s;
	getString (s, desc, tab);
	out << s << std::endl;
}

void Comment::updateUnrollInformation (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

void Comment::updateRegisterName (int nbrIterationsAdvance)
{
	(void) nbrIterationsAdvance;
}

bool Comment::isSimilar (const Statement *stmt) const
{
	//Easy comparison
	if (stmt == this)
		return true;

	//Oh well, we have more to do:
	//1) check if stmt is a comment
	const Comment *c = dynamic_cast<const Comment*> (stmt);

	//If not a comment, we are finished
	if (c == NULL)
	{
		return false;
	}

	//2) Compare comments
	return c->comment == comment;
}
