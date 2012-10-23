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
  @file PluginGate.cpp
  @brief The PluginGate plugin is in this file 
 */


#include <cassert>
#include <iostream>
#include <string.h>

#include "Driver.h"
#include "PassElement.h"
#include "PluginGateInfo.h"

//-----------------------------------------------------------------------------
//-------------------------------Local variables-------------------------------
//-----------------------------------------------------------------------------

static const std::string ourName = "Gate plugin";		/** < @brief The plugin name */

//-----------------------------------------------------------------------------
//-----------------------------Functions definition----------------------------
//-----------------------------------------------------------------------------

extern "C" const std::string &myPluginName (void)
{
	return ourName;
}

bool myGate (const Description *desc, const PassElement *elem)
{
	//If nothing, why not?
	if (elem == NULL)
		return true;

	bool res = elem->gate (desc, elem->getKernel ());
	const Pass *pass = elem->getPass ();

	//If original gate says no, so do we
	if (res == false)
		return false;

	return (pass->getName () != "Immediate selection before unroll");
}

extern "C" void pluginInit (Driver *driver, const Description *desc)
{
	//PluginGate
	SPluginGateInfo plugInfo1 = {myPluginName, myGate};
	driver->setPluginGate (plugInfo1);

    (void) desc;
}
