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
 @file PluginPass.h
 @brief The PluginPass class header is in this file
 */

#ifndef H_PLUGINPASS
#define H_PLUGINPASS

#include "Pass.h"
#include "PluginGateInfo.h"
#include "PluginPassInfo.h"

#include <string>
#include <vector>

//Advanced declarations
class Description;
class Driver;
class Kernel;
class PassElement;

/**
 * @class PluginPass
 * @brief The PluginPass defines the plugin passes
 */
class PluginPass : public Pass
{
	protected:
		const std::string &(*namePtr) (void); 									/**< @brief Pointer function to getName */
		bool (*gatePluginPass) (const Description*, const Kernel*); 			/**< @brief Pointer function to gate */
		std::vector<PassElement*>* (*entryPtr) (PassElement*, Description*); 	/**< @brief Pointer function to entry */
		bool (*gatePluginGate) (const Description*, const PassElement*); 		/**< @brief Pointer function to plugin gate information */

	public:
		/**
		 * @brief Constructor
		 */
		PluginPass (const SPluginPassInfo &passInfo);

		/**
		 * @brief Destructor
		 */
		virtual ~PluginPass (void);

		/**
		 * @brief Get name of pass
		 * @return the name of the pass
		 */
		const std::string &getName (void) const;

		/**
		 * @brief Gate function
		 * @param desc the Description of the input (default value is NULL)
		 * @param kernel the kernel we care about (default value is NULL)
		 * @return whether or not we perform the pass
		 */
		virtual bool gate (const Description *desc = NULL, const Kernel *kernel = NULL) const;

		/**
		 * @brief Entry function
		 * @param passElem the PassElement
		 * @param desc the Description of the input (default value is NULL)
		 * @return the future jobs to be done by the PassEngine
		 */
		std::vector<PassElement*>* entry (PassElement *passElem, Description *desc) const;

		/**
		 * @brief Adding pass
		 * @param driver the driver of the system
		 * @param info information about the plugin pass
		 */
		void pluginAddPass (Driver *driver, const SPluginPassInfo &info);
};
#endif
