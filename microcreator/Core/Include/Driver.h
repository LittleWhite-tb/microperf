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
 @file Driver.h
 @brief The Driver class header is in this file
 */

#ifndef H_DRIVER
#define H_DRIVER

#include "PluginPass.h"

#include <string>
#include <vector>

//Advanced declarations
class Description;
class Pass;
class PassEngine;

/**
 * @class Driver
 * @brief The Driver of the program
 */
class Driver
{
	protected:
		PassEngine *passEngine; /**< @brief The pass engine */
		Description *description; /**< @brief The description of the input */

	public:
		/**
		 * @brief Constructor
		 */
		Driver (void);

		/**
		 * @brief Constructor with description
		 * @param desc the Description
		 */
		Driver (Description *desc);

		/**
		 * @brief Destructor
		 */
		virtual ~Driver (void);

		/**
		 * @brief Initialization function
		 */
		void init (void);

		/**
		 * @brief Drive the passes
		 */
		void drive (void);

		/**
		 * @brief Add the general passes 
		 */
		void addGeneralPasses (void);

		/**
		 * @brief Add a Pass to the engine at the end
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPass (Pass *newPass);

		/**
		 * @brief Add a Pass after another one to the engine
		 * @param after name of the Pass we wish to add after to
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPassAfter (const std::string &after, Pass *newPass);

		/**
		 * @brief Add a Pass before another one to the engine
		 * @param before name of the Pass we wish to add before to
		 * @param newPass the Pass to add
		 * @return whether or not things were done correctly
		 */
		bool addPassBefore (const std::string &before, Pass *newPass);

		/**
		 * @brief Replace the Pass with another one
		 * @param name name of the Pass we wish to replace
		 * @param newPass the Pass to replace
		 * @return whether or not things were done correctly
		 */
		bool replacePass (const std::string &name, Pass *newPass);

		/**
		 * @brief remove the Pass
		 * @param name name of the Pass we wish to remove 
		 * @return whether or not things were done correctly
		 */
		bool removePass (const std::string &name);

		/**
		 * @brief Set the description
		 * @param d the description
		 */
		void setDescription (Description *d);

		/**
		 * @brief Set the gate of the plugin
		 * @param gateInfo the gate information
		 * @return the value of the gate
		 */
		bool setPluginGate (const SPluginGateInfo &gateInfo);

		/**
		 * @brief Get the current PassEngine 
		 * @return the PassEngine
		 */
		PassEngine *getPassEngine (void);
};
#endif
