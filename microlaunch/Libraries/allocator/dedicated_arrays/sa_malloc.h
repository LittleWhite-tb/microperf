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

#ifndef SA_MALLOC_H
#define SA_MALLOC_H

#include <stddef.h>

/**
	@brief Returns an array aligned on pagesize + alignment.
	@param Size Size of the desired array. For internal reasons, more memory than required will be allocated.
	@param Alignment Desired alignment
	@return An aligned array in case of success, NULL otherwise
*/
void* sa_malloc(size_t size, int alignment);

/**
	@brief Frees an array previously allocated with sa_malloc AND NOTHING ELSE
	@param ptr Address of the array to deallocate
*/
void sa_free(void* ptr);


#endif
