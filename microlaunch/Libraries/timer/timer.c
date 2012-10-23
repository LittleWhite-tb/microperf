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
   PROJECT : Kernel_Mem_Benchmark (kerbe)
   SOURCE  : timer.c
   MODULE  : Performance Counters
   AUTHOR  : Christophe Lemuet
   UPDATED : 09/09/2003 (DD/MM/YYYY)
   UPDATED : 20/09/2005 (DD/MM/YYYY) JT.Acquaviva. Add X86 timer with rdtsc() function
============================================================================*/

#include <stdlib.h>

#include "Rdtsc.h"
#include "timer.h"
       
static __inline__ unsigned long long getticks(void)
{
   unsigned long long ret;
   rdtscll(ret);
   return ret;
}

void *evaluationInit (void)
{
	return NULL;
}

int evaluationClose (void *data)
{
	(void) data;
	return EXIT_SUCCESS;
}

double evaluationStart (void *data)
{
	(void) data;
	return  getticks();
}

double evaluationStop (void *data)
{
	(void) data;
	return  getticks();
}

