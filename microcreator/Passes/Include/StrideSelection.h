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
  @file StrideSelection.h
  @brief The StrideSelection pass header is in this file 
 */

#ifndef H_STRIDESELECTION
#define H_STRIDESELECTION

#include "Pass.h"
#include "RandomRepeat.h"

#include <string>
#include <vector>

class Description;
class Kernel;
class InductionOperand;
class PassElement;

/**
 * @class StrideSelection
 * @brief The StrideSelection class
 */
class StrideSelection:public Pass
{      	
	protected:
		/**
		 * @brief Handle working on each Kernel 
		 * @param kernels the Kernel vector
		 */
		void handlePass (std::vector<Kernel *> &kernels) const;

		/**
		 * @brief Handle a particular Kernel
		 * @param kernels the Kernel vector
		 * @param outer the outer Kernel
		 * @param kernel the Kernel 
		 */
		void handleKernel (std::vector<Kernel *> &kernels, const Kernel *outer, Kernel *kernel) const;

		/**
		 * @brief Fill the vector of Kernels with the segment information
		 * @param kernel the Kernel 
		 * @param outer the outer Kernel 
		 * @param newKernel the list of new Kernel 
		 * @param segment the segment of InductionOperand vector
		 */
		void fillKernels (const Kernel *kernel, const Kernel *outer, std::vector<Kernel *> &newKernel, std::vector <const InductionOperand*> &segment) const;

		/**
		 * @brief Handle this kernel: choose the strides and generate new Kernels
		 * @param kernels the list of new Kernel 
		 * @param kernel the Kernel 
		 * @param outer the outer Kernel 
		 */
		void handleKernelChooseStride (std::vector<Kernel *> &kernels, const Kernel *outer, const Kernel *kernel) const;

		/**
		 * @brief Get the next repetition values
		 * @param repetitions The number of instructions we want for each instruction
		 */
		void getNextRepetition (std::vector<SRandomRepeat> &repetitions) const;

		/**
		 * @brief Fill the vector of Kernels with a randomized segment information
		 * @param kernel the Kernel 
		 * @param outer the outer Kernel 
		 * @param newKernels the generated Kernel vector
		 * @param segment the randomized segment of InductionOperand vector
		 * @param repetitions The number of instructions we want for each instruction
		 */
		void generateRandomKernels (const Kernel *kernel, const Kernel *outer, std::vector<Kernel *> &newKernels,
				std::vector<const InductionOperand*> &segment,
				std::vector<SRandomRepeat> &repetitions) const;

	public:
		/**
		 * @brief Constructor
		 */
		StrideSelection (void);

		/**
		 * @brief Destructor
		 */
		virtual ~StrideSelection (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass
		 */
		virtual std::vector <PassElement *> *entry (PassElement *pe, Description *desc) const;
};
#endif
