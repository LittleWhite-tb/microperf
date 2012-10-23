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
 @file RegisterOperand.h
 @brief The RegisterOperand class header is in this file
 */

#ifndef H_REGISTEROPERAND
#define H_REGISTEROPERAND

#include "Operand.h"

//Advanced declaration
class InductionOperand;

#include <string>
#include <vector>

/**
 * @class sRegNames
 * @brief struct sRegNames is used for the register names
 */
typedef struct sRegNames
{
		std::string virtualReg;
		std::string physicalReg;
		const InductionOperand *induction;
		
} SRegNames;

/**
 * @class RegisterOperand
 * @brief The RegisterOperand of an Instruction
 */
class RegisterOperand : public Operand
{
	protected:
		std::vector<SRegNames> regs; 	/**< @brief Register names */
		unsigned int chosen; 			/**< @brief Which is the chosen register */
		static std::string empty; 		/**< @brief An empty holder for the returns */

	public:
		/**
		 * @brief Constructor
		 */
		RegisterOperand (void);

		/**
		 * @brief Destructor
		 */
		virtual ~RegisterOperand (void);

		/**
		 * @brief Push virtual name
		 * @param s the new virtual name
		 */
		void pushVirtualName (const std::string &s);

		/**
		 * @brief Push physical name
		 * @param s the new physical name
		 */
		void pushPhysicalName (const std::string &s);

		/**
		 * @brief Push virtual and physical name
		 * @param virtualName the new virtual name
		 * @param physicalName the new physical name
		 */
		void pushRegisterNames (const std::string &virtualName, const std::string &physicalName);

		/**
		 * @brief Choose one of the names
		 * @param idx the chosen index
		 * @param physical is the chosen index the physical one or not
		 */
		void setChosenName (unsigned int idx, bool physical = false);

		/**
		 * @brief Add the string of this operand to this string
		 * @param s the string we wish to fill
		 */
		virtual void addString (std::string &s) const;

		/** 
		 * @brief Copy this operand
		 * @return a copy of this operandSRegNames
		 */
		virtual Operand *copy (void) const;

		/**
		 * @brief Copy information
		 * @param source the RegisterOperand we are copying from
		 */
		void copyInformation (const RegisterOperand *source);

		/**
		 * @brief Update register name 
		 * @param nbrIterationsAdvance the number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance);

		/**
		 * @brief Returns the name of this register, either it's the physical name or the virtual one
		 * @param idx the index of the name we are looking for (default is -1 to say, give us the chosen one)
		 * @return the reference to the name 
		 */
		const std::string &getName (int idx = -1) const;

		/**
		 * @brief Returns the virtual register (can return empty string if not yet populated)
		 * @param idx the index we are interested in
		 * @return the reference to the virtual register or "" if v is not valid
		 */
		const std::string &getVirtualRegister (unsigned int idx) const;

		/**
		 * @brief Returns the chosen virtual register (can return empty string if not yet populated)
		 * @return the reference to the chosen virtual register or "" if v is not valid (default = 0)
		 */
		const std::string &getVirtualRegister (void) const;

		/**
		 * @brief Returns the physical register (can return empty string if not yet populated)
		 * @param idx the index we are interested in
		 * @return the reference to the physical register or "" if v is not valid
		 */
		const std::string &getPhysicalRegister (unsigned int idx) const;

		/**
		 * @brief Returns the chosen physical register (can return empty string if not yet populated)
		 * @return the reference to the chosen physical register or "" if v is not valid (default = 0)
		 */
		const std::string &getPhysicalRegister (void) const;

		/**
		 * @brief Sets the physical register 
		 * @param physicalReg the value we are interested in
		 */
		void setPhysicalRegister (const std::string &physicalReg);

		/**
		 * @brief Sets the virtual register 
		 * @param virtualReg the value we are interested in
		 */
		void setVirtualRegister (const std::string &virtualReg);

		/**
		 * @brief Do we already have a physical register ?
		 * @return whether or not we have a physical register
		 */
		bool hasPhysicalRegister (void) const;

		/**
		 * @brief Get the stride (actually 0 for this class)
		 * @return int value of the stride 
		 */
		virtual int getStride (void) const;

		/**
		 * @brief Handle the case where this is an Induction variable
         * @param findNewInductins do we find induction variables anyway?
		 * @param kernel the Kernel
		 */
		virtual void handleInductionVariables (const Kernel *kernel, bool findNewInductions = true);

		/**
		 * @brief Get the offset
		 * @return int value of the offset 
		 */
		virtual int getOffset (void) const;

		/**
		 * @brief Fill the vector with RegisterOperand
		 * @param registers the register vector we wish to fill
		 */
		virtual void fillRegisters (std::vector<RegisterOperand*> &registers);

		/**
		 * @brief Get register names
		 * @return vector of register names
		 */
		const std::vector<SRegNames> &getRegs (void) const;

		/**
		 * @brief Which is the chosen register
		 * @return int value of the choosen
		 */
		unsigned int getChosen (void) const;

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};

#endif
