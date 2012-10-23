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
 @file RegularRegisterOperand.h
 @brief The RegularRegisterOperand class header is in this file
 */

#ifndef H_REGULARREGISTEROPERAND
#define H_REGULARREGISTEROPERAND

#include "RegisterOperand.h"

/**
 * @class RegularRegisterOperand
 * @brief RegularRegisterOperand is the operand of the regular register
 */
class RegularRegisterOperand : public RegisterOperand
{
	protected:
		bool inOrder; /**< @brief Do we want the progression to be in order? */

	public:

		/**
		 * @brief Constructor
		 * @param RegOpVirtualName the virtual name of the RegularRegisterOperand
		 * @param RegOpPhysicalName the physical name of the RegularRegisterOperand
		 * @param min the min integer value of the register
		 * @param max the max integer value of the register
		 * @param order in order or not
		 * @param current is current index in the (min/max) range. If -1, then it takes the value of min (default : -1)
		 */
		RegularRegisterOperand (const std::string &RegOpVirtualName, const std::string &RegOpPhysicalName = "", int min = -1, int max = -1, bool order = true, int current = -1);

		/**
		 * @brief Destructor
		 */
		~RegularRegisterOperand (void);

		/**
		 * @brief Update register name if it is a physical node
		 * @param nbrIterationsAdvance the number of iterations in advance
		 */
		virtual void updateRegisterName (int nbrIterationsAdvance);

		/** 
		 * @brief Copy this operand
		 * @return a copy of this operand
		 */
		virtual Operand *copy (void) const;

		/**
		 * @brief Fill the register names
		 * @param name the name of the register
		 * @param isVirtual is the name the virtual name or not?
		 * @param min the minimum number
		 * @param max the maximum number
		 */
		void fillRegisterNames (const std::string &name, bool isVirtual, int min, int max);

		/**
		 * @brief Do we want the progression to be in order
		 * return bool value of order progression
		 */
		bool getInOrder (void) const;

		/**
		 * @brief compare this Operand to another one
		 * @param op the Operand we wish to compare to
		 * @return whether or not op is similar to this one
		 */
		virtual bool isSimilar (const Operand *op) const;
};
#endif
