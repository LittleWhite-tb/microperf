
/*
Copyright (C) 2010 BEYLER Jean Christophe

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

#ifndef		H_LOGGING
#define		H_LOGGING

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
		 std::string fileName;                  /**< @brief File name used for the log */
         int ignore;                            /**< @brief Ignore value */

         std::vector <std::string> savedLines;  /**< @brief Saved lines for print out at the end of the program */
         unsigned int maxLines;                 /**< @brief Maximum lines used for the logging system */

         /**
          * @brief Constructor
          * @param maxLines Initializer for member maxLines, default is 10
          */
		 Logging (int maxLines = 10);

		 ~Logging (void);                       /**< @brief Destructor */
         Logging (const Logging&);              /**< @brief Prevent copy-construction */
         Logging& operator=(const Logging&);    /**< @brief Prevent assignment */

         /**
          * @brief Internal logging function
          * @param ignore Ignore level
          * @param vl list of what is to be logged, char* list, NULL terminated
          */
         void logIt (int ignore, va_list vl);

         /**
          * @brief Internal get instance function
          * @return The pointer to the singleton pointer 
          */
         static Logging *getInst (void);

         static Logging *ourInst;               /**< @brief Our instance */
         static bool firstTime;                 /**< @brief First time we use this logging system */

	public:
        static void shutDown (void);            /**< @brief ShutDown function */ 

        /**
         * @brief Logging function
         * @param ignore level of log entry, enables the logger to ignore certain logs
         */
		static void log (int ignore, ...);

        /**
         * @brief set log file
         * @param name is the file name for the new log file
         */
		static void setLogFile (std::string name);

        /**
         * @brief set ignore value
         * @param value new value for the ignore variable
         */
        static void setIgnore (int value);
};
#endif
