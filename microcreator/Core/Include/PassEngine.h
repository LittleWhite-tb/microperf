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
 @file PassEngine.h
 @brief The PassEngine class header is in this file
 */

#ifndef H_PASSENGINE
#define H_PASSENGINE

#include <string>
#include <list>

#include "PluginPass.h"

//Advanced declaration
class Description;
class Pass;
class PassElement;

/**
 * @class PassEngine
 * @brief The PassEngine simply handles the list of passes and calls one after the other
 */
class PassEngine
{
	protected:
		std::list<Pass*> listPasses; 	/**< @brief List of passes */
		SPluginGateInfo gateInfo; 		/**< @brief The gate defined by a plugin */

		/**
		 * @brief Debug the passes
		 * @param fileName the file we wish to output to
		 * @param pe the PassElement
		 * @param desc the Description
		 */
		void debug (const std::string &fileName, PassElement *pe, Description *desc) const;

	public:
		/**
		 * @brief Constructor
		 */
		PassEngine (void);

		/**
		 * @brief Destructor
		 */
		~PassEngine (void);

		/**
		 * @brief Find the pass after the one we are looking for
		 * @param name the name of the pass
		 * @return the Pass we are looking for, NULL if not found
		 */
		Pass *findNextPass (const std::string &name);

		/**
		 * @brief Get the list of passes 
		 * return the list of passes
		 */
		std::list<Pass*> &getListPasses (void);

		/**
		 * @brief Add a Pass to the engine at the end
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPass (Pass *newPass);

		/**
		 * @brief Add a Pass after another one to the engine
		 * @param nameAfter name of the Pass we wish to add after to
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPassAfter (const std::string &nameAfter, Pass *newPass);

		/**
		 * @brief Add a Pass before another one to the engine
		 * @param nameBefore name of the Pass we wish to add before to
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPassBefore (const std::string &nameBefore, Pass *newPass);

		/**
		 * @brief Replace the Pass with another one
		 * @param passToReplace name of the Pass we wish to replace
		 * @param newPass the Pass to replace
		 * @return whether or not things were done correctly
		 */
		bool replacePass (const std::string &passToReplace, Pass *newPass);

		/**
		 * @brief Remove the Pass
		 * @param passToRemove of the Pass we wish to remove
		 * @return whether or not things were done correctly
		 */
		bool removePass (const std::string &passToRemove);

		/**
		 * @brief Drives the pass system
		 * @param desc the Description of the input
		 */
		void drivePasses (Description *desc);

		/**
		 * @brief Set the gateInfo 
		 * @param gateInfo the info contained in the pluginGate
		 */
		bool setPluginGate (const SPluginGateInfo &gateInfo);
};
#endif
