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
  @file RegisterAllocation.h
  @brief The RegisterAllocation pass header is in this file 
 */

#ifndef H_REGISTERALLOCATION
#define H_REGISTERALLOCATION

#include "Pass.h"

//Advanced declaration
class Description;
class Operand;
class RegisterOperand;
class Statement;

/**
 * @class RegisterAllocation
 * @brief The RegisterAllocation handles the register allocation between virtual and physical registers
 */
class RegisterAllocation:public Pass
{
	protected:
		/**
		 * @brief Handle the Kernel
		 * @param kernel the Kernel we are interested in
		 * @param desc the Description of the input (default value is NULL)
		 */
		void handleKernel (Kernel *kernel, const Description *desc) const;

		/**
		 * @brief Handle the Statement
		 * @param instruction the Statement we are interested in
		 * @param desc the Description of the input (default value is NULL)
		 */
		void handleStatement (Statement *instruction, const Description *desc) const;

		/**
		 * @brief Handle the Operand
		 * @param operand the Operand we are interested in
		 * @param desc the Description of the input (default value is NULL)
		 */
		void handleOperand (Operand *operand, const Description *desc) const;

		/**
		 * @brief Handle the RegisterOperand
		 * @param operand the RegisterOperand we are interested in
		 * @param desc the Description of the input (default value is NULL)
		 */
		void handleRegisterOperand (RegisterOperand *operand, const Description *desc) const;

	public:
		/**
		 * @brief Constructor
		 */
		RegisterAllocation (void);

		/**
		 * @brief Destructor
		 */
		virtual ~RegisterAllocation (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};

#endif
