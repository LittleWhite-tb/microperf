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
  @file ImmediateOperand.h
  @brief The ImmediateOperand class header is in this file 
 */

#ifndef H_IMMEDIATEOPERAND
#define H_IMMEDIATEOPERAND

#include "Operand.h"

#include <string>

/**
 * @class ImmediateOperand
 * @brief The ImmediateOperand of an Instruction
 */
class ImmediateOperand:public Operand
{
	protected:
		long immediate;
		long minImmediate;
		long maxImmediate;
		long progressImmediate;

		/**
		 * @brief Initialization function
		 */
		void init (void);

	public:
		/**
		 * @brief Constructor
		 * @param imm Immediate value
		 */
		ImmediateOperand (long imm);

		/**
		 * @brief Constructor
		 * @param minImm minimum immediate value
		 * @param maxImm maximmum immediate value
		 * @param progressImm progress immediate value
		 */
		ImmediateOperand (long minImm, long maxImm, long progressImm);

		/**
		 * @brief Destructor
		 */
		virtual ~ImmediateOperand (void);

		/**
		 * @brief Return the value of immediate operande
		 * @return the value of immediate operande
		 */
		long getValImmediate (void) const;

		/**
		 * @brief Set the value of immediate operand
		 * @param imm  immediate operand value 
		 */
		void setValImmediate (long imm);

		/**
		 * @brief Get the min value of immediate operand
		 * @return the min value of immediate operand
		 */
		long getMinImmediate (void) const;

		/**
		 * @brief Set the min value of immediate operand
		 * @param minImm immediate operand min value  
		 */
		void setMinImmediate (long minImm);

		/**
		 * @brief Get the maximum value of immediate operand
		 * @return the maximum value of immediate operand
		 */
		long getMaxImmediate (void) const;

		/**
		 * @brief Set the max value of immediate operand
		 * @param maxImm immediate operand maximum value  
		 */
		void setMaxImmediate (long maxImm);

		/**
		 * @brief Get the progress value of immediate operand
		 * @return the progress value of immediate operand
		 */
		long getProgressImmediate (void) const;

		/**
		 * @brief Set the progress value of immediate operande
		 * @param progressImm the progress value of immediate operand
		 */
		void setProgressImmediate (long progressImm);                   

		/**
		 * @brief Add the string of this operand to this string
		 * @param s the string we wish to fill
		 */
		virtual void addString (std::string &s) const;

		/** 
		 * @brief Copy this operand
		 * @return a copy of this operand
		 */
		virtual Operand *copy (void) const;

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};

#endif
