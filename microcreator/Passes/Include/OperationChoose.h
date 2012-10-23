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
  @file OperationChoose.h
  @brief The OperationChoose pass header is in this file 
 */

#ifndef H_OPERATIONCHOOSE
#define H_OPERATIONCHOOSE

#include "Pass.h"

//Advanced declaration
class Kernel;

/**
 * @class OperationChoose
 * @brief The OperationChoose defines what to do for one pass
 */
class OperationChoose:public Pass
{
	protected:
		bool beforeUnroll;  /**< @brief Is this pass before the unrolling pass or not? */

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
		 * @brief Handle the operation choose of a kernel
		 * @param kernels the vector of Kernels 
		 * @param kernel the Kernel we are considering to unroll
		 * @param outer the outer Kernel we are considering to unroll
		 * @param start the start index (Default = 0)
		 */
		void handleOpChoose (std::vector <Kernel*> &kernels, const Kernel *kernel, Kernel *outer, unsigned int start = 0) const;

	public:
		/**
		 * @brief Constructor
		 * @param bu is this pass before the unrolling pass or not?
		 */
		OperationChoose (bool bu);

		/**
		 * @brief Destructor
		 */
		virtual ~OperationChoose (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};

#endif
