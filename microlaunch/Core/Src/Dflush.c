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
/*============================================================================
   PROJECT : K1D_Mem_Benchmark
   SOURCE  : dflush.c
   MODULE  : Flush 
   AUTHOR  : Karine Brifault & Christophe Lemuet
   UPDATED : 09/09/2003 (DD/MM/YYYY)
============================================================================*/

#include <stdio.h>

#include "Dflush.h"

double readDummyArray (double *dummy, unsigned long sizeMB)
{
    unsigned long end = sizeMB / sizeof (*dummy), i;
    double a = 0;
    
    for (i = 0 ; i < end ; i++)
    {
        a += dummy[i];
	}

    dummy[0] = a/end;

    return a;
}

