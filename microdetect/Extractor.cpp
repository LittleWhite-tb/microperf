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
#include <sstream>

#include "ArgsInfo.h"
#include "Extractor.h"
#include "Logging.h"

enum ExtractState {OUT_OF_FUNCTION, IN_FUNCTION};

Extractor::Extractor (char *s, char *d)
{
    source = s;
    destination = d;
}

Extractor::~Extractor (void)
{
    std::vector<ArgsInfo*>::const_iterator it;
    for (it = args.begin (); it != args.end (); it++)
    {
        delete *it;
    }
    args.clear ();
}

bool Extractor::drive (void)
{   
    //Open the source file
    std::ifstream in (source, std::ios::in);

    if (in.is_open () == false)
    {
        Logging::log (0, "Extract: Error opening source: ", source, NULL);
        return false;
    }

    //Open destination file
    std::ofstream out (destination, std::ios::out);

    if (out.is_open () == false)
    {
        Logging::log (0, "Extract: Error opening destination: ", source, NULL);
        return false;
    }

    //Now read lines
    std::string line;

    std::string start = "globl";
    //First fill vector
    while (std::getline (in, line))
    {
        if (line.find (start) != line.npos)
        {
            ArgsInfo *ai = new ArgsInfo ();
            //Just push out an empty one
            args.push_back (ai);
        }
    }
    //Now close it and open it again
    in.close ();
    in.open (source, std::ios::in);
    assert (in.is_open ());

    unsigned int idx;
    int state = OUT_OF_FUNCTION;
    while (std::getline (in, line))
    {
        switch (state)
        {
            case OUT_OF_FUNCTION:
                if (line.find (start) != line.npos)
                {
                    //Figure out which index it is
                    unsigned long under = line.find_last_of ('_');

                    assert (under != line.npos);

                    line = line.substr (under + 1);

                    //Ok now transform it in integer
                    std::istringstream iss (line);
                    iss >> idx;

                    state = IN_FUNCTION;
                }
                break;
            case IN_FUNCTION:
                if (line.find ("mov") != line.npos)
                {
                    std::string firstArg, secondArg;
                    assert (idx < args.size ());
                    args[idx]->separateLine (line);
                    state = OUT_OF_FUNCTION;
                }
                break;
            default:
                assert (0);
                break;
        }
    }

    //Now sort the arguments
    sortArgs ();

    //All good
    return true;
}

void Extractor::sortArgs (void)
{
    std::vector<ArgsInfo*>::const_iterator it;

    //Nothing to do
    if (args.size () == 0)
        return;

    //Check first
    std::string destination = args[0]->getDestination ();
    for (it = args.begin (); it != args.end (); it++)
    {
        ArgsInfo *elem = *it;
        if (destination.compare (elem->getDestination ()) != 0)
            break;
    }

    //If finished loop, we are done
    if (it == args.end ())
        return;

    //Otherwise, we must switch them but first, let's be paranoid and check
    std::string source = args[0]->getSource ();

    //Ok now we must go through the arguments and see which is which
    for (it = args.begin (); it != args.end (); it++)
    {
        ArgsInfo *elem = *it;
        if (source.compare (elem->getSource ()) != 0)
            break;
    }
    assert (it == args.end ());

    //Now switch
    for (it = args.begin (); it != args.end (); it++)
    {
        ArgsInfo *elem = *it;
        elem->switchThem ();
    }
}

std::string Extractor::getParameter (unsigned int i)
{
    assert (i < args.size ());

    return args[i]->getSource ();
}

unsigned int Extractor::getNumParameters (void)
{
    return args.size ();
}
