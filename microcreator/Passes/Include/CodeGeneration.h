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
  @file CodeGeneration.h
  @brief The CodeGeneration pass header is in this file 
 */

#ifndef H_CODEGENERATION
#define H_CODEGENERATION

#include <fstream>

#include "Pass.h"

//Advanced declaration
class Description;
class Kernel;

/**
 * @class CodeGeneration
 * @brief The CodeGeneration defines what to do for one pass
 */
class CodeGeneration:public Pass
{
	protected:
		/**
		 * @brief Output a Kernel
		 * @param out the output 
		 * @param kernel the Kernel we wish to output
		 * @param desc the Description
		 * @param tabulation the tabulation value (Default : 1)
		 */
		void outputKernel (std::ofstream &out, const Kernel *kernel, const Description *desc, unsigned int tabulation) const;

		/**
		 * @brief output a file
		 * @param out the output description in which we will copy the file
		 * @param name the file we are going to open and copy out
		 */
		void outputFile (std::ofstream &out, const std::string &name) const;

        /**
         * @brief Get kernel information
         * @param oss the output string stream
         * @param kernel the Kernel
         * @param desc the Description
         */
		void getKernelInfo (std::ostringstream &oss, const Kernel *kernel, const Description *desc) const;

	public:
		/**
		 * @brief Constructor
		 */
		CodeGeneration (void);

		/**
		 * @brief Destructor
		 */
		virtual ~CodeGeneration (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass 
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};

#endif
