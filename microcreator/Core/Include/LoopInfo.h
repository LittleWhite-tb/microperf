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
 @file LoopInfo.h
 @brief The sRegName and sLoopInfo structs are in this file
 */

#ifndef H_LOOPINFO
#define H_LOOPINFO

/**
 * @class sLoopInfo
 * @brief struct sLoopInfo is used for the loop information
 */

/**
 * @class sRegName
 * @brief struct sRegNames is used for the register name
 */
typedef struct sRegName
{
		std::string name;
		unsigned int offset;
} SRegName;

typedef struct sLoopInfo
{
		std::string induction;
		unsigned int increment;
		unsigned int min;
		unsigned int max;
		std::string minStr;
		std::string maxStr;
		SRegName reg;

} SLoopInfo;

#endif
