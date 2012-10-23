/*
 Copyright (C) 2011 Exascale Computing Research

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
 @file Logging.h
 @brief The Logging class header is in this file
 */

#ifndef	H_LOGGING
#define	H_LOGGING

#include <stdarg.h>
#include <string>
#include <vector>

/**
 * @class Logging
 * @brief Logging class handles anything that is necessary for logging information
 */
class Logging
{
	protected:
		std::string fileName; 	/**< @brief File name used for the log */
		int ignore; 			/**< @brief Ignore value */

		std::vector<std::string> savedLines; 	/**< @brief Saved lines for print out at the end of the program */
		unsigned int maxLines; 					/**< @brief Maximum lines used for the logging system */

		/**
		 * @brief Constructor
		 * @param maxLines Initializer for member maxLines, default is 10
		 */
		Logging (int maxLines = 10);

		/**
		 * @brief Destructor
		 */
		~Logging (void);

		/**
		 * @brief Prevent copy-construction
		 */
		Logging (const Logging&);

		/**
		 * @brief Prevent assignment
		 */
		Logging& operator= (const Logging&);

		/**
		 * @brief Internal logging function
		 * @param ignore Ignore level
		 * @param log list of what is to be logged, char* list, NULL terminated
		 */
		void logIt (int ignore, va_list log);

		/**
		 * @brief Internal get instance function
		 * @return The pointer to the singleton pointer 
		 */
		static Logging *getInst (void);

		/**
		 * @brief Our instance
		 */
		static Logging *ourInst;

		 /**
		  * @brief First time we use this logging system
		  */
		static bool firstTime;

	public:
		/**
		 * @brief ShutDown function
		 */
		static void shutDown (void);

		/**
		 * @brief Logging function
		 * @param ignore level of log entry, enables the logger to ignore certain logs
		 */
		static void log (int ignore, ...);

		/**
		 * @brief set log file
		 * @param name is the file name for the new log file
		 * @param firstTime is this the first time (Default to true), this decides whether we append or not
		 */
		static void setLogFile (const std::string &name, bool firstTime = true);

		/**
		 * @brief set ignore value
		 * @param value new value for the ignore variable
		 */
		static void setIgnore (int value);
};
#endif
