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
  @file PluginRemovePass.cpp
  @brief The PluginRemovePass plugin is in this file 
 */

#include <iostream>
#include <cassert>
#include <sstream>

#include "Driver.h"
#include "Kernel.h"
#include "Logging.h"
#include "PassElement.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                           This plugin remove pass
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


extern "C" void pluginInit (Driver *driver, const Description *desc)
{
	//PluginPass
	driver->removePass ("Pass unroll");
    (void) desc;
}

