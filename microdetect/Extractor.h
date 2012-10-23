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

#ifndef	H_EXTRACTOR
#define	H_EXTRACTOR

#include <string>
#include <vector>

//Forward declaration
class ArgsInfo;

/**
 * @class Extractor
 * @brief Extractor extracts the information from the source files
 */
class Extractor
{
	public:
         /**
          * @brief Constructor
          * @param source is the source filename
          * @param dest is the destination filename
          */
         Extractor (char *source, char *dest);

         /**
          * @brief Destructor
          */
         ~Extractor (void);

         /** 
          * @brief main driver function 
          * @return whether or not it succeeded
          */
         bool drive (void); 

         /**
          * @brief Get parameter name
          * @return the string representing the parameter
          */
         std::string getParameter (unsigned int i);

         /**
          * @brief Get parameter number
          * @return the number of parameter strings we have
          */
         unsigned int getNumParameters (void);

	protected:
         char *source;          /**< @brief Source filename */
         char *destination;     /**< @brief Destination filename */
         std::vector <ArgsInfo *> args; /**< @brief Vector of argument information */

         //Sort the arguments
         void sortArgs (void);
};
#endif
