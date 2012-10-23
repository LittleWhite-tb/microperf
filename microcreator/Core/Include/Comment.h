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
 @file Comment.h
 @brief The Comment class header is in this file
 */

#ifndef H_COMMENT
#define H_COMMENT

#include "Statement.h"

//Advanced declarations
class Description;

/**
 * @class Comment
 * @brief The Comment class
 */
class Comment : public Statement
{
	protected:
		std::string comment;	//the comment string to add

	public:

		/**
		 * @brief Constructor
		 */
		Comment (void);

		/**
		 * @brief Constructor
		 * @param com the comment to display
		 */
		Comment (const std::string &com);

		/**
		 * @brief Destructor
		 */
		~Comment (void);

		/**
		 * @brief Set the comment string
		 * @param s string to be set
		 */
		void setComment (const std::string &s);

		/**
		 * @brief Get the comment
		 * @return the comment string
		 */
		const std::string getComment (void) const;

		/**
		 * @brief adding the string to the previous one
		 * @param s string to be added
		 */
		void addString (std::string &s);

		/**
		 * @brief clear the content of the comment
		 */
		void clearComment (void);

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
		virtual bool getString (std::string &s, const Description *desc,
				unsigned int tab = 1) const;

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
		 * @brief compare this Statement to another one
		 * @param stmt the statemen we wish to compare to
		 * @return whether or not stmt is similar to this one
		 */
		virtual bool isSimilar (const Statement *stmt) const;

		/**
		 * @brief Copy the Kernel (actually calls the copy function with true)
		 * @return the new Kernel
		 */
		virtual Statement *copy (void) const;

		/**
		 * @brief Copy the information and call the copy information of their parent    
		 * @param stmt The statement
		 */
		virtual void copyInformation (const Statement *stmt);

		/**
		 * @brief Debug function
		 * @param out the output stream
		 * @param desc the Description
		 * @param tab tabulation
		 */
		virtual void debug (std::ofstream &out, const Description *desc,
				int tab) const;
};
#endif
