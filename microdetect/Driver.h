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

#ifndef	H_DRIVER
#define	H_DRIVER

#include <string>

/**
 * @class Driver
 * @brief Driver class handles the main driver of the program
 */
class Driver
{
	public:
         /**
          * @brief Constructor
          * @param source is the source filename
          * @param dest is the destination filename
          */
         Driver (char *source, char *dest);

         /**
          * @brief Destructor
          */
         ~Driver (void);

         /** 
          * @brief main driver function 
          * @return whether or not it succeeded
          */
         bool drive (void); 

	protected:
         char *source;          /**< @brief Source filename */
         char *destination;     /**< @brief Destination filename */
};
#endif
