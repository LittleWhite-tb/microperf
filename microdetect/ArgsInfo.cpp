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

#include "ArgsInfo.h"

#include <assert.h>
#include <iostream>

ArgsInfo::ArgsInfo (void)
{
}

ArgsInfo::~ArgsInfo (void)
{
}

void ArgsInfo::separateLine (const std::string &line)
{
    //Now get first letter
    unsigned int i = 0;
    unsigned int len = line.size ();
    unsigned int start, end;

    while (i < len && (line[i] < 'a' || line[i] >'z'))
        i++;
    assert (i < len);

    //Now skip word
    while (i < len && (line[i] >= 'a' && line[i] <= 'z'))
        i++;
    assert (i < len);

    //Now skip spaces
    while (i < len && (line[i] == ' ' || line[i] == '\t'))
        i++;
    assert (i < len);

    //Now we have first argument
    start = i;
    //Now get end of first argument, search for the comma
    while (i < len && line[i] != ',')
        i++;
    assert (i < len);
    end = i;

    source = line.substr (start, end - start);

    //Now skip comma
    i++;

    //Now we have second
    destination = line.substr (i);
}

std::string ArgsInfo::getSource (void)
{
    return source;
}

std::string ArgsInfo::getDestination (void)
{
    return destination;
}

void ArgsInfo::switchThem (void)
{
    std::string tmp = source;
    source = destination;
    destination = tmp;
}
