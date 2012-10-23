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
 @file PluginPassInfo.h
 @brief The sPluginPassInfo struct is in this file
 */

#ifndef H_PLUGINPASSINFO
#define H_PLUGINPASSINFO

//How to insert the plugin pass
enum pluginPositioning
{
	PASS_POS_UNKNOWN, PASS_POS_INSERT_AFTER, PASS_POS_INSERT_BEFORE, PASS_POS_REPLACE
};

/**
 * @class sPluginPassInfo
 * @brief struct sPluginPassInfo is used for plugin passes
 */
typedef struct sPluginPassInfo
{
		const std::string &(*name) (void);
		bool (*gate) (const Description*, const Kernel*);
		std::vector<PassElement*>* (*entry) (PassElement*, Description*);
		const char* passRef;
		pluginPositioning pluginPos;
} SPluginPassInfo;
#endif
