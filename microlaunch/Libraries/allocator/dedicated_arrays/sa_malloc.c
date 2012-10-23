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
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sa_malloc.h"

void* sa_malloc (size_t size, int alignment)
{
	char* ptr, *tmp;
	size_t c_size;
	unsigned int modulo_decalage;

	if (size <= 0)
	{
		return NULL;
	}

	assert (alignment % 4 == 0 && alignment < getpagesize ());

	c_size = size + alignment+ sizeof (void*);

	ptr = malloc (c_size + getpagesize ());

	tmp = ptr + sizeof (void*);

	modulo_decalage = ( (unsigned long) tmp) % getpagesize ();

	modulo_decalage = getpagesize () - modulo_decalage;

	tmp += modulo_decalage;

	tmp += alignment;

	memcpy (tmp - sizeof (void*), &ptr, sizeof (void*));

	return tmp;
}

void sa_free (void* ptr)
{
	void** tmp = ptr;
	free (* (tmp - 1));
}
