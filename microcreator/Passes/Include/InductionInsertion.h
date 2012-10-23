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
  @file InductionInsertion.h
  @brief The InductionInsertion pass header is in this file 
 */

#ifndef H_INDUCTIONINSERTION
#define H_INDUCTIONINSERTION

#include "Pass.h"

#include <string>

//Forward declarations
class Description;
class PassElement;

/**
 * @class InductionInsertion
 * @brief The InductionInsertion class
 */
class InductionInsertion:public Pass
{      	
	protected:
		/**
		 * @brief Handle a Kernel
		 * @param kernel the Kernel we are interested in
		 */
		void handleKernel (Kernel *kernel) const;

	public:
		/**
		 * @brief Constructor
		 */
		InductionInsertion (void);

		/**
		 * @brief Destructor
		 */
		virtual ~InductionInsertion (void);

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by this Pass
		 */
		virtual std::vector <PassElement *> *entry (PassElement *pe, Description *desc) const ;
};
#endif
