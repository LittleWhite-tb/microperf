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

/* For the kernelMDL call (when <nbSizes> node is specified) */
unsigned int entryPoint2 (unsigned int nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors)
{
	(void) nbVectors;
	(void) vectorSizes;
	(void) elemSize;
	(void) vectors;
	
	return 0;
}


unsigned int entryPoint (unsigned int n, void *tab, unsigned int elemSize)
{
    float *ptr = tab;
    unsigned int i;
    double sum = 5;

    for (i=0; i < n; i ++)
    {
        sum += ptr[i];
    }

    ptr[0] = sum;
    
    (void) elemSize;
    return i;
}
