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

#ifndef	H_ARGSINFO
#define	H_ARGSINFO

#include <string>

/**
 * @class ArgsInfo
 * @brief ArgsInfo extracts the information from the source files
 */
class ArgsInfo
{
	public:
         /**
          * @brief Constructor
          */
         ArgsInfo (void);

         /**
          * @brief Destructor
          */
         ~ArgsInfo (void);

         /** 
          * @brief fills the two arguments with the line information
          * @param line the line used
          */
         void separateLine (const std::string &line); 

         /**
          * @brief get the source string
          * @returns the source string
          */
         std::string getSource (void);
         /**
          * @brief get the destination string
          * @returns the destination string
          */
         std::string getDestination (void);
         /**
          * @brief Switch the two strings
          */
         void switchThem (void);

	protected:
         std::string source;          /**< @brief source argument */
         std::string destination;     /**< @brief destination arguemnt */
};
#endif
