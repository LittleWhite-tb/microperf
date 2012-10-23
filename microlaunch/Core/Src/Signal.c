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


#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "LinkList.h"
#include "Signal.h"
#include "Log.h"

//We have a static link list for the signals
static SLinkList *signals = NULL;

/**
 * @brief Delete a element of the link list
 * @param data is a struct sigaction
 */
void
signalDelete (void *data)
{
    free (data), data = NULL;
}

void 
pushSignalHandler (void (*fct) (int, siginfo_t*, void*))
{
    struct sigaction signal;
    struct sigaction *oldsignal;

    if (fct == NULL)
        return;

    memset (&signal, 0, sizeof (signal));
    signal.sa_sigaction = fct;
    signal.sa_flags = SA_SIGINFO;

    oldsignal = malloc (sizeof (*oldsignal));
    assert (oldsignal);
    memset (oldsignal, 0, sizeof (*oldsignal));

    sigaction (SIGSEGV, &signal, oldsignal);
    sigaction (SIGPIPE, &signal, oldsignal);
    sigaction (SIGUSR1, &signal, oldsignal);
    sigaction (SIGABRT, &signal, oldsignal);

    //First creation
    if (signals == NULL)
    {
        signals = linkList_create (
                    NULL,
                    signalDelete);
    }

    linkList_addHead (signals, oldsignal);
}

void
popSignalHandler (void)
{
    if (signals != NULL)
    {
        SLinkListNode *oldSignalNode = linkList_getHead (signals);

        if (oldSignalNode != NULL && oldSignalNode->data != NULL)
        {
            sigaction (SIGSEGV, oldSignalNode->data, NULL);
        }

        linkList_removeHead (signals);
        
        //Check if empty
        if (linkList_isEmpty (signals))
        {
            linkList_destroy (signals);
        }
    }
}
