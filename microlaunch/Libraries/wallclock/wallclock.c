/*
   Copyright (C) 2012 Exascale Research Center

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

#include <sys/time.h>
#include <stdlib.h>

extern void *evaluationInit (void)
{
	return NULL;
}

extern int evaluationClose (void *data)
{
	(void) data;
	
	return 0;
}

extern double evaluationStart (void *data)
{
	struct timeval tv;
	
	(void) data;
	
	gettimeofday(&tv, NULL);
	return tv.tv_usec + tv.tv_sec * 1000000.;
}

extern double evaluationStop (void *data)
{
	(void) data;
	
	return evaluationStart(NULL);
}

