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
 @file MemoryOperand.h
 @brief The MemoryOperand class header is in this file
 */

#ifndef H_MEMORYOPERAND
#define H_MEMORYOPERAND

#include <string>

#include "Operand.h"

//Advanced declaration
class RegisterOperand;

/**
 * @class MemoryOperand
 * @brief The MemoryOperand of an Instruction
 */
class MemoryOperand : public Operand
{
	protected:
		//A MemoryOperand contains an offset and a register
		int offset; 			/**< @brief The offset of the memory access */
		RegisterOperand *reg; 	/**< @brief The register */

	public:
		/**
		 * @brief Constructor
		 */
		MemoryOperand ();

		/**
		 * @brief Constructor
		 * @param regOp the RegisterOperand
		 * @param offset the offset for this access
		 */
		MemoryOperand (RegisterOperand *regOp, int offset);

		/**
		 * @brief Destructor
		 */
		virtual ~MemoryOperand (void);

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
		 * @brief Get the offset
		 * @return returns the offset
		 */
		int getOffset (void) const;

		/**
		 * @brief Set the offset
		 * @param offset new offset
		 */
		void setOffset (int offset);

		/**
		 * @brief Get the RegisterOperand
		 * @return the RegisterOperand
		 */
		const RegisterOperand* getRegister (void) const;

		/**
		 * @brief Get the RegisterOperand without the const
		 * @return the RegisterOperand
		 */
		RegisterOperand* getModifiableRegister (void);

		/**
		 * @brief Set a new RegisterOperand
		 * @param regOp the new RegisterOperand, deletes the old one
		 */
		void setRegister (RegisterOperand *regOp);

		/**
		 * @brief Update unroll information
		 * @param nbrIterationsAdvance the number of iterations in advance
		 */
		virtual void updateUnroll (int nbrIterationsAdvance);

		/**
		 * @brief Handle the case where this is an Induction variable
         * @param findNewInductins do we find induction variables anyway?
		 * @param kernel the Kernel
		 */
		virtual void handleInductionVariables (const Kernel *kernel, bool findNewInductions = true);

		/**
		 * @brief Fill the vector with RegisterOperand
		 * @param registers the register vector we wish to fill
		 */
		virtual void fillRegisters (std::vector<RegisterOperand*> &registers);

		/**
		 * @brief Update register name if it is a physical node
		 * @param nbrIterationsAdvance the number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance);

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};

#endif
