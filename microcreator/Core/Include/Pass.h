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
 @file Pass.h
 @brief The Pass class header is in this file
 */

#ifndef H_PASS
#define H_PASS

#include <string>
#include <vector>

//Advanced declarations
class Description;
class Kernel;
class PassElement;

/**
 * @class Pass
 * @brief The Pass defines what to do for one pass
 */
class Pass
{
	protected:
		std::string name; /**< @brief Name of the pass */

	public:
		/**
		 * @brief Constructor
		 */
		Pass (void);

		/**
		 * @brief Destructor
		 */
		virtual ~Pass (void);

		/**
		 * @brief Get name of pass
		 * @return the name of the pass
		 */
		virtual const std::string &getName (void) const;

		/**
		 * @brief Gate function
		 * @param desc the Description of the input (default value is NULL)
		 * @param kernel the kernel we care about (default value is NULL)
		 * @return whether or not we perform the pass
		 */
		virtual bool gate (const Description *desc = NULL, const Kernel *kernel = NULL) const;

		/**
		 * @brief Entry function
		 * @param pe the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by the PassEngine
		 */
		virtual std::vector<PassElement*> *entry (PassElement *pe, Description *desc = NULL) const;
};
#endif
