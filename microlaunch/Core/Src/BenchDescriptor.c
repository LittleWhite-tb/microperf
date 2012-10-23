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

#include <stdio.h>
#include <stdlib.h>
#include "BenchDescriptor.h"
#include "Defines.h"

unsigned long kernel1 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, unsigned) = func;
	unsigned long size = 0;
	void *x = NULL;
    unsigned long res = 0;
    
    if (nbVectors > 0)
    {
    	size = vectorSizes[0];
    	x = vectors[0];
    }
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel2 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel3 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel4 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    void *a = vectors[3];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, a, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel5 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    void *a = vectors[3];
    void *b = vectors[4];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, a, b, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel6 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    void *a = vectors[3];
    void *b = vectors[4];
    void *c = vectors[5];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, a, b, c, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel7 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, void*, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    void *a = vectors[3];
    void *b = vectors[4];
    void *c = vectors[5];
    void *d = vectors[6];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, a, b, c, d, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernel8 (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{
	unsigned long (*entryPoint) (unsigned long, void*, void*, void*, void*, void*, void*, void*, void*, unsigned) = func;
	unsigned long size = vectorSizes[0];
	void *x = vectors[0];
    void *y = vectors[1];
    void *z = vectors[2];
    void *a = vectors[3];
    void *b = vectors[4];
    void *c = vectors[5];
    void *d = vectors[6];
    void *e = vectors[7];
    unsigned long res = 0;
    
    if (entryPoint != NULL)
    {
    	res = entryPoint (size, x, y, z, a, b, c, d, e, elemSize);
    }
    
    (void) nbVectors;
    return res;
}

unsigned long kernelMDL (unsigned long nbVectors, unsigned long *vectorSizes, unsigned elemSize, void **vectors, void *func)
{   
	unsigned long (*entryPoint) (unsigned long, unsigned long*, unsigned, void **) = func;
    unsigned long res = 0;

    if (entryPoint != NULL)
    {
	    res = entryPoint (nbVectors, vectorSizes, elemSize, vectors);
    }
    
    return res;
}
