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
 @file Operation.cpp
 @brief The Operation class is in this file
 */

#include "Operation.h"

Operation::Operation (OperationType opType, const std::string &name)
{
	type = opType;
	op_name = name;
}

Operation::~Operation (void)
{
}

void Operation::addString (std::string &s) const
{
	s += op_name;
}

Operation* Operation::copy (void) const
{
	return new Operation (type, op_name);
}

OperationType Operation::getType (void) const
{
	return type;
}

const std::string &Operation::getName (void) const
{
	return op_name;
}

void Operation::setName (std::string &name)
{
	op_name = name;
}

bool Operation::isSimilar (const Operation *op) const
{
	//Easy comparison
	if (op == this)
		return true;

	return getType () == op->getType () && op_name == op->op_name;
}
