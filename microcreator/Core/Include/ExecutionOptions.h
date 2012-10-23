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
 @file ExecutionOptions.h
 @brief The ExecutionOptions class header is in this file
 */

#ifndef H_EXECUTIONOPTIONS
#define H_EXECUTIONOPTIONS

#include "Description.h"
#include "PluginPass.h"

enum PluginType
{
	UNKNOWN_FILE, LIBRARY_FILE, SOURCE_FILE
};

/**
 * @class ExecutionOptions
 * @brief The ExecutionOptions handles the different options of execution
 */
class ExecutionOptions
{
	protected:
		static struct option long_options[];
		const char *pluginPath; 				/**< @brief The plugin path */
		bool pluginCmd; 						/**< @brief If the plugin is defined in command-line? */
		const char* passRef; 					/**< @brief The pass referenced by plugin */
		pluginPositioning pluginPos; 			/**< @brief Positioning of the plugin */
		bool handlingOkay; 						/**< @brief If the handling of the command-system is done without problem */

	public:

		/**
		 * @brief Constructor
		 * @param argc the argc
		 * @param argv the argv
		 * @param desc the description
		 * @param driver the driver
		 */
		ExecutionOptions (int argc, char **argv, Description *desc, Driver *driver);

		/**
		 * @brief Destructor
		 */
		~ExecutionOptions (void);

		/**
		 * @brief Get the type of plugin (can be a library (.c) or a source file (.cpp))
		 * @param str the option passed to the tool
		 * @param folder the folder containing the plugin 
		 * @param file plugin file
		 */
		PluginType getPluginFileType (const std::string &str,
				std::string &folder, std::string &file);

		/**
		 * @brief Get the name of plugin with the path to this plugin
		 * @param optarg the the argument passes in command-line
		 * @param pluginStr the name and the path to the plugin
		 * @return bool if there are problem, return false else true
		 */
		bool getPluginName (const std::string optarg, std::string &pluginStr);

		/**
		 * @brief Display the different options of the command-line system
		 */
		void differentOptions (void);

		/**
		 * @brief Does the handling of the command-system is done without problem?
		 * @return the value of handleOkay
		 */
		bool getHandlingOkay (void) const;

		/**
		 * @brief Set the value of handleOkay
		 * @param val the value of handleOkay
		 */
		void setHandlingOkay (bool val);
};
#endif
