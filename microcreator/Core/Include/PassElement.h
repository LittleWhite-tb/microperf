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
 @file PassElement.h
 @brief The PassElement class header is in this file
 */

#ifndef H_PASSELEMENT
#define H_PASSELEMENT

#include <string>
#include <vector>

//Advanced declaration 
class Description;
class Kernel;
class Pass;

/**
 * @class PassElement
 * @brief The PassElement defines one element that is going through the pass engine, it contains all the information between passes
 */
class PassElement
{
	protected:
		Pass *current; 	/**< @brief Current pass */
		Kernel *kernel; /**< @brief Current kernel */

	public:
		/**
		 * @brief Constructor
		 */
		PassElement (void);

		/**
		 * @brief Destructor
		 */
		~PassElement (void);

		/**
		 * @brief Set current Pass
		 */
		void setPass (Pass *pass);

		/**
		 * @brief Test the pass's gate
		 * @param desc the Description of the input (default value is NULL)
		 * @param kernel the kernel we care about (default value is NULL)
		 * @return the result of the pass's gate
		 */
		bool gate (const Description *desc = NULL, const Kernel * kernel = NULL) const;

		/**
		 * @brief Perform the entry of the Pass
		 * @param desc the Description of the input (default value is NULL)
		 * @return the result of the entry call
		 */
		std::vector<PassElement *> *entry (Description *desc = NULL);

		/**
		 * @brief Get current Pass name
		 * @return the pass name, "" if Pass is NULL
		 */
		const std::string &getPassName (void) const;

		/**
		 * @brief Get Pass 
		 * @return the pass, NULL is possible
		 */
		const Pass *getPass (void) const;

		/**
		 * @brief Get Kernel
		 * @return the kernel, NULL is possible
		 */
		Kernel *getKernel (void) const;

		/**
		 * @brief Set the kernel
		 * @param kernel the Kernel
		 */
		void setKernel (Kernel *kernel);
};
#endif
