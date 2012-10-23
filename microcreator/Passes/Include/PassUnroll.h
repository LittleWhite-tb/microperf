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
  @file PassUnroll.h
  @brief The PassUnroll pass header is in this file 
 */

#ifndef H_PASSUNROLL
#define H_PASSUNROLL

#include "Pass.h"

#include <string>

class Kernel;
class Description;
class PassElement;

/**
 * @class PassUnroll
 * @brief The PassUnroll unroll pass for the output
 */
class PassUnroll:public Pass
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
		void handleKernel (std::vector<Kernel*> &kernels, Kernel *outer, Kernel *kernel) const;

		/**
		 * @brief Handle the unroll of a kernel
		 * @param kernels the vector of Kernels 
		 * @param kernel the Kernel we are considering to unroll
		 * @param outer the outer Kernel we are considering to unroll
		 */
		void handleUnroll (std::vector <Kernel*> &kernels, Kernel *kernel, Kernel *outer) const;

		/**
		 * @brief Handle the unroll of a kernel
		 * @param kernels the vector of Kernels 
		 * @param outer the outer Kernel we are considering to unroll
		 * @param kernel the Kernel we are considering to unroll
		 * @param i the kernel to handle
		 * @param unroll if we handle the kernel or not 
		 * @param linkedKernel wether the kernel is linke or not
		 */
		void unrollIt (std::vector <Kernel*> &kernels, Kernel *outer, Kernel *kernel, int i, bool unroll, bool linkedKernel) const;

		/**
		 * @brief Wehther the kernel contains instructions or not? 
		 * @param kernel the Kernel we are considering to check
		 * @return bool if the kernel contains instructions 
		 */	
		bool kernelContainsInstructions (const Kernel *kernel) const;

		/**
		 * @brief find the linked kernel
		 * @param outer the kernel containing all the kernels
		 * @param name the name of our linked kernel
		 * @return the linked kernel 
		 */
		const Kernel *findOurLinked (const Kernel *outer, const std::string &name) const;

	public:
		/**
		 * @brief Constructor
		 */
		PassUnroll (void);

		/**
		 * @brief Destructor
		 */
		virtual ~PassUnroll (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass
		 */
		virtual std::vector <PassElement *> *entry (PassElement *pe, Description *desc) const;
};
#endif
