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
  @file ImmediateSelection.h
  @brief The ImmediateSelection pass header is in this file 
 */

#ifndef H_IMMEDIATESELECTION
#define H_IMMEDIATESELECTION

#include "Pass.h"

//Advanced declaration
class Kernel;
class Instruction;
class ImmediateOperand;

/**
 * @class ImmediateSelection
 * @brief The ImmediateSelection defines what to do for one pass
 */
class ImmediateSelection:public Pass
{
	protected:

		bool beforeUnroll; 		/** < @brief Is this pass before or after the unrolling pass? */

		/**
		 * @brief Handle values of immediate operande
		 * @param pool the pool of Pass Elements
		 * @param outer the most outer Kernel 
		 * @param kernel the Kernel we are considering
		 * @param start which Instruction do we start considering (default is 0)
		 */
		void handleImmediate (std::vector <PassElement *> *pool, const Kernel *outer, Kernel *kernel, unsigned int start = 0) const;

		/**
		 * @brief find the linked instruction
		 * @param outer the kernel containing all the kernels
		 * @param name the name of our linked instruction
		 * @return the linked instruction
		 */
		const Instruction *findOurLinked (const Kernel *outer, const std::string &name) const;

		/**
		 * @brief Update the immediate operand corresponding to an instruction 
		 * @param linked The Instruction we care about
		 * @param operand The ImmediateOperand we care about
		 */
		void updateImmediateOperand (const Instruction *linked, ImmediateOperand *operand) const;

	public:
		/**
		 * @brief Constructor
		 * @param bu Is the pass before the unrolling pass or not?
		 */
		ImmediateSelection (bool bu);

		/**
		 * @brief Destructor
		 */
		virtual ~ImmediateSelection (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};

#endif
