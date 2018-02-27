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

#include <assert.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "Driver.h"
#include "Extractor.h"
#include "Logging.h"

#include <map>
#include <vector>
#include <unistd.h>

Driver::Driver (char *s, char *d)
{
    source = s;
    destination = d;
}

Driver::~Driver (void)
{
}

bool Driver::drive (void)
{   
    //First thing we do: compile the file
    char name[] = "/tmp/microXXXXXX";
    int res = mkstemp (name);
    close (res);

    //Now create it
    char buf[512];
    res = snprintf (buf, sizeof (buf), "gcc -O3 -Wall -Wextra -S %s -o %s", source, name);

    Logging::log (1, "Going to perform system call: ", buf, NULL);
    unsigned int ures = res; 
    assert (ures < sizeof (buf));

    res = system (buf);
    assert (res == EXIT_SUCCESS);

    //Ok we now have it, fill out the information now
    Extractor extract (name, destination);

    if (extract.drive () == false)
    {
        Logging::log (0, "Extraction has failed", NULL);
        return false;
    }

    std::ofstream out (destination, std::ios::out);
    assert (out.is_open () == true);

    //Ok now we can print out the information
    unsigned int len = extract.getNumParameters ();

    out << "<description>" << std::endl;
    out << "\t<register_association>" << std::endl;
    for (unsigned int i = 0; i < len; i++)
    {
         out << "\t\t<register>" << std::endl;
         out << "\t\t\t<virtual>r" << i << "</virtual>" << std::endl;
         out << "\t\t\t<physical>" << extract.getParameter (i) << "</physical>" << std::endl;
         out << "\t\t</register>" << std::endl;
    }

	for (unsigned int i = 10; i < 14; i++)
    {
         out << "\t\t<register>" << std::endl;
         out << "\t\t\t<virtual>r" << i << "</virtual>" << std::endl;
         out << "\t\t\t<physical>\%r" << i << "</physical>" << std::endl;
         out << "\t\t</register>" << std::endl;
    }

    for (unsigned int i = 0; i < 16; i++)
    {
         out << "\t\t<register>" << std::endl;
         out << "\t\t\t<virtual>v" << i << "</virtual>" << std::endl;
         out << "\t\t\t<physical>\%xmm" << i << "</physical>" << std::endl;
         out << "\t\t</register>" << std::endl;
    }
    out << "\t</register_association>" << std::endl;

    out << "</description>" << std::endl;

    out.close ();

    //Remove file
    remove (name);

    //All good
    return true;
}

