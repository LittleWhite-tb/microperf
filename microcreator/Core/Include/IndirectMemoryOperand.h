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
 @file IndirectMemoryOperand.h
 @brief The IndirectMemoryOperand class header is in this file
 */

#ifndef H_INDIRECTMEMORYOPERAND
#define H_INDIRECTMEMORYOPERAND

#include <string>

#include "MemoryOperand.h"

//Advanced declaration
class RegisterOperand;

/**
 * @class IndirectMemoryOperand
 * @brief The IndirectMemoryOperand of an Instruction
 */
class IndirectMemoryOperand : public MemoryOperand
{
	protected:
		RegisterOperand *index; /**< @brief The index register */
		int multiplier; /**< @brief The multiplier */

	public:
		/**
		 * @brief Constructor
		 */
		IndirectMemoryOperand ();

		/**
		 * @brief Constructor
		 * @param base the base RegisterOperand
		 * @param i the indirect RegisterOperand
		 * @param offset the offset for this access
		 * @param multiplier the multiplier for this access
		 */
		IndirectMemoryOperand (RegisterOperand *base, RegisterOperand *i, int offset, int multiplier);

		/**
		 * @brief Destructor
		 */
		virtual ~IndirectMemoryOperand (void);

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
		 * @brief Get the multiplier
		 * @return returns the multiplier
		 */
		int getMultiplier (void) const;

		/**
		 * @brief Set the multiplier
		 * @param off new multiplier
		 */
		void setMultiplier (int off);

		/**
		 * @brief Get the base RegisterOperand
		 * @return the base RegisterOperand
		 */
		const RegisterOperand* getBaseRegister (void) const;

		/**
		 * @brief Set a new index RegisterOperand
		 * @param reg the new index RegisterOperand, deletes the old one
		 */
		void setIndexRegister (RegisterOperand *reg);

		/**
		 * @brief Set a new base RegisterOperand
		 * @param reg the new base RegisterOperand, deletes the old one
		 */
		void setBaseRegister (RegisterOperand *reg);

		/**
		 * @brief Get the base RegisterOperand without the const
		 * @return the base RegisterOperand
		 */
		RegisterOperand* getModifiableBaseRegister (void);

		/**
		 * @brief Get the index RegisterOperand
		 * @return the RegisterOperand
		 */
		const RegisterOperand* getIndexRegister (void) const;

		/**
		 * @brief Get the index RegisterOperand without the const
		 * @return the RegisterOperand
		 */
		RegisterOperand* getModifiableIndexRegister (void);

		/**
		 * @brief Update unroll information
		 * @param nbrIterationsAdvance the number of iterations in advance
		 */
		virtual void updateUnroll (int nbrIterationsAdvance);

		/**
		 * @brief Handle the case where one of our registers might be an induction variable
		 * @param kernel the Kernel
		 */
		virtual void handleInductionVariables (const Kernel *kernel, bool findNewInductions = true);

		/**
		 * @brief Fill the vector with RegisterOperand
		 * @param registers the register vector we wish to fill
		 */
		virtual void fillRegisters (std::vector<RegisterOperand*> &registers);

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};

#endif
