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
  @file StatementSelection.h
  @brief The StatementSelection pass header is in this file 
 */

#ifndef H_STATEMENTSELECTION
#define H_STATEMENTSELECTION

#include "Pass.h"
#include "RandomRepeat.h"

#include <vector>

//Advance declaration
class Instruction;
class Kernel;
class Statement;

/**
 * @class StatementSelection
 * @brief The StatementSelection defines what to do for one pass
 */
class StatementSelection:public Pass
{
	protected:
		/**
		 * @brief Handle working on each Kernel
		 * @param kernels the Kernel vector
		 */
		void handlePass (std::vector<Kernel*> &kernels) const;

		/**
		 * @brief Handle a particular Kernel
		 * @param kernels the Kernel vector
		 * @param outer the outer Kernel
		 * @param kernel the Kernel
		 */
		void handleKernel (std::vector<Kernel*> &kernels, const Kernel *outer, Kernel *kernel) const;

		/**
		 * @brief Handle the selection of a kernel
		 * @param kernels the vector of Kernels
		 * @param kernel the Kernel we are considering to unroll
		 * @param outer the outer Kernel we are considering to unroll
		 */
		void handleSelection (std::vector <Kernel*> &kernels, const Kernel *kernel, const Kernel *outer) const;

		/**
		 * @brief Fill the vector of Kernels with the Kernel information
		 * @param kernels the Kernel vector
		 * @param kernel Kernel we wish to fill with
		 */
		void fillKernels (std::vector<Kernel *> &kernels, const Kernel *kernel) const;

		/**
		 * @brief Fill the vector of Kernels with the Kerenl information
		 * @param kernels the Kernel vector
		 * @param kernel the Kernel we are using
		 */
		void fillKernelsWithStatements (std::vector<Kernel *> &kernels, const Kernel *kernel) const;

		/**
		 * @brief Fill the vector of Kernels with a randomized segment information
		 * @param kernels the Kernel vector
		 * @param kernel the Kernel we are using
		 * @param combination do we do the combination or the permutation
		 */
		void fillKernelsRandomize (std::vector<Kernel *> &kernels, const Kernel *kernel, bool combination) const;

		/**
		 * @brief Get the next repetition values
		 * @param repetitions The number of instructions we want for each instruction
		 */
		void getNextRepetition (std::vector<SRandomRepeat> &repetitions) const;

		/**
		 * @brief Fill the vector of Kernels with a randomized segment information
		 * @param kernels the Kernel vector
		 * @param newKernels the generated Kernel vector
		 * @param kernel the Kernel we are using
		 * @param repetitions The number of instructions we want for each instruction
		 * @param combination do we do the combination or the permutation
		 */
		void generateRandomKernels (std::vector<Kernel *> &kernels,
				std::vector<Kernel *> &newKernels,
				const Kernel *kernel,
				std::vector<SRandomRepeat> &repetitions,
				bool combination) const;

		/**
		 * @brief Fill the vector of Kernels with a randomized segment information
		 * @param kernels the Kernel vector
		 * @param newKernels the generated Kernel vector
		 * @param kernel the Kernel we are using
		 * @param repetitions The number of instructions we want for each instruction
		 * @param permutations the current permutation to use
		 * @param max the number of elements in permutations
		 * @param current the index currently looked at
		 * @param combination do we do the combination or the permutation
		 */
		void generateRandomKernelsPermute (std::vector<Kernel *> &kernels,
				std::vector<Kernel *> &newKernels, const Kernel *kernel,
				std::vector<SRandomRepeat> &repetitions, unsigned int *permutations,
				unsigned int max, unsigned int current,
				bool combination) const;

		/**
		 * @brief Fill the vector of Kernels with a randomized segment information
		 * @param kernels the Kernel vector
		 * @param newKernels the generated Kernel vector
		 * @param kernel the Kernel we are using
		 * @param repetitions The number of instructions we want for each instruction
		 * @param permutations the current permutation to use
		 * @param max the number of elements in permutations
		 */
		void fillRandomKernel (std::vector<Kernel *> &kernels,
				std::vector<Kernel *> &newKernels, const Kernel *kernel,
				std::vector<SRandomRepeat> &repetitions, unsigned int *permutations, unsigned int max) const;

		/**
		 * @brief Function handles the Statement with a given Kernel and adds to the list
		 * @param inst the Statement we want to add to the Kernel
		 * @param kernel the Kernel we have as a basis
		 * @param list the list of new Kernel
		 */
		void handleStatement (const Statement *inst, Kernel *kernel, std::vector<Kernel *> &list) const;

		/**
		 * @brief Function handles the Instruction with a given Kernel and adds to the list
		 * @param inst the Instruction we want to add to the Kernel
		 * @param kernel the Kernel we have as a basis
		 * @param list the list of new Kernel
		 */
		void handleInstruction (const Instruction *inst, Kernel *kernel, std::vector<Kernel *> &list) const;

		/**
		 * @brief Find the instruction index we have
		 * @param repetitions repetition information
		 * @param search the index we have
		 * @param modifyValue do we want to modify search with the offset in this instruction (default: false)
		 * @return the index to the repetition vector
		 */
		unsigned int findStatement (std::vector<SRandomRepeat> &repetitions, unsigned int &search, bool modifyValue = false) const;

		/**
		 * @brief Find the next element
		 * @param repetitions repetition information
		 * @param current the current index
		 * @param combination do we do the combination or the permutation
		 * @return return the index of the next element
		 */
		unsigned int findNextElement (std::vector<SRandomRepeat> &repetitions, unsigned int current, bool combination) const;

		/**
		 * @brief Are all the kernels colored
		 * @param kernel the kernel
		 * @param value the color we wish to see everywhere
		 * @return return whether all is colored with value
		 */
		bool kernelsAllColored (const Kernel *kernel, unsigned int value) const;

	public:
		/**
		 * @brief Constructor
		 */
		StatementSelection (void);

		/**
		 * @brief Destructor
		 */
		virtual ~StatementSelection (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;

		/**
		 * @brief Handling the selection
		 * @param kernels the vector of Kernel we wish to obtain
		 * @param pool the Statement Kernel we are using
		 */
		void handleSelection (std::vector <Kernel *> &kernels, const Kernel *pool) const;
};

#endif
