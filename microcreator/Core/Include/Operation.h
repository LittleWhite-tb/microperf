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
 @file Operation.h
 @brief The Operation class header is in this file
 */

#ifndef H_OPERATION
#define H_OPERATION

#include <string>
#include <vector>

enum OperationType
{
	OP_TYPE_UNKNOWN, OP_TYPE_NOP, OP_TYPE_LOAD, OP_TYPE_ADD, OP_TYPE_MAX_OPERATION_TYPE
};

/**
 * @class Operation
 * @brief The Operation of an Instruction
 */
class Operation
{
	protected:
		OperationType type; 	/**< @brief Type of the operation */
		std::string op_name; 	/**< @brief Operation name */

	public:
		/**
		 * @brief Constructor
		 * @param opType the OperationType
		 * @param name the assembly operation to use
		 */
		Operation (OperationType opType, const std::string &name);

		/**
		 * @brief Destructor
		 */
		virtual ~Operation (void);

		/**
		 * @brief Adds the string of this operation to the string
		 * @param s the string we wish to fill
		 */
		void addString (std::string &s) const;

		/** 
		 * @brief Copy this operation
		 * @return a copy of this operation
		 */
		virtual Operation *copy (void) const;

		/**
		 * @brief Get type
		 * @return the type
		 */
		OperationType getType (void) const;

		/**
		 * @brief Set the Operation name
		 * @param name the Operation name
		 */
		void setName (std::string &name);

		/**
		 * @brief Gete the Operation name
		 * return the name of the Operation
		 */
		const std::string &getName (void) const;

		/**
		 * @brief compare this Operation to another one
		 * @param op the Operation we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operation *op) const;
};
#endif
