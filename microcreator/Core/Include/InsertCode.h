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
 @file InsertCode.h
 @brief The InsertCode class header is in this file
 */

#ifndef H_INSERTCODE
#define H_INSERTCODE

#include "Statement.h"

//Advanced declarations
class Description;

/**
 * @class InsertCode
 * @brief The InsertCode class
 */
class InsertCode : public Statement
{
	protected:
		std::string fileName;
		std::string instruction;
		bool insertTabs;

	public:

		/**
		 * @brief Constructor
		 */
		InsertCode (void);

		/**
		 * @brief Constructor
		 * @param comment the comment to display
		 * @param instr the instruction to display
		 * @param insertTab do we insert the tabulations when getting the code? (Default: false)
		 */
		InsertCode (const std::string &comment, const std::string &instr, bool insertTab = false);

		/**
		 * @brief Destructor
		 */
		~InsertCode (void);

		/**
		 * @brief Get the file name
		 * @return the file name
		 */
		const std::string getFileName (void) const;

		/**
		 * @brief Set the file name
		 * @param name the file name
		 */
		void setFileName (const std::string &name);

		/**
		 * @brief Get the instruction
		 * @return the instruction
		 */
		const std::string getInstruction (void) const;

		/**
		 * @brief Set the instruction
		 * @param name the instruction
		 */
		void setInstruction (const std::string &name);

		/**
		 * @brief adding the string to the previous one
		 * @param s string to be added
		 */
		void addString (const std::string &s);

		/**
		 * @brief initializing the string
		 */
		void init (void);

		/** From Statement **/

		/**
		 * @brief Get a String
		 * @param s the string we wish to fill
		 * @param desc the description
		 * @param tab the tabulation (default: 1)
		 * @return whether or not this succeeded
		 */
		virtual bool getString (std::string &s, const Description *desc, unsigned int tab = 1) const;

		/**
		 * @brief Set Unroll information for this Instruction
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateUnrollInformation (int nbrIterationsAdvance);

		/**
		 * @brief Update RegisterOperand's names
		 * @param nbrIterationsAdvance number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance);

		/**
		 * @brief Copy the Kernel (actually calls the copy function with true)
		 * @return the new Kernel
		 */
		virtual Statement *copy (void) const;

		/**
		 * @brief   
		 * @param stmt
		 */
		virtual void copyInformation (const Statement *stmt);

		/**
		 * @brief Debug function
		 * @param outStream the output stream
		 * @param desc the Description
		 * @param tab tabulation
		 */
		virtual void debug (std::ofstream &outStream, const Description *desc, int tab) const;

		/**
		 * @brief compare this Statement to another one
		 * @param stmt the statemen we wish to compare to
		 * @return whether or not stmt is similar to this one
		 */
		virtual bool isSimilar (const Statement *stmt) const;
};
#endif
