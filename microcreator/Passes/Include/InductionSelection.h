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
  @file InductionSelection.h
  @brief The InductionSelection pass header is in this file 
 */

#ifndef H_INDUCTIONSELECTION
#define H_INDUCTIONSELECTION

#include "Pass.h"

#include <string>

class Description;
class Kernel;
class Operand;
class PassElement;
class Statement;

/**
 * @class InductionSelection
 * @brief The InductionSelection class
 */
class InductionSelection:public Pass
{      	
	protected:
		/**
		 * @brief Handle the this kernel
		 * @param kernel the Kernel we are considering
		 */
		void handleKernel (Kernel *kernel) const;

		/**
		 * @brief Handle the Statement
		 * @param instruction the Statement we are interested in
		 * @param kernelription the Kernel of the input (default value is NULL)
		 */
		void handleStatement (Statement *instruction, Kernel *kernelription) const;

		/**
		 * @brief Handle the Operand
		 * @param operand the Operand we are interested in
		 * @param kernel the Kernel of the input (default value is NULL)
		 */
		void handleOperand (Operand *operand, Kernel *kernel) const;

	public:
		/**
		 * @brief Constructor
		 */
		InductionSelection (void);

		/**
		 * @brief Destructor
		 */
		virtual ~InductionSelection (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass
		 */
		virtual std::vector <PassElement *> *entry (PassElement *pe, Description *desc) const ;
};
#endif
