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


#ifndef PERSONALIZED_ALLOCATOR_H
#define PERSONALIZED_ALLOCATOR_H

/**
	* Initiator: prepares future function calls if need be
	* @param ptr a structure to pass on arguments in the initializer might want
*/
int personalized_malloc_init(void* ptr);

/**
	* Allocator: returns a pointer pointing to an array of usable bytes, or NULL in case of error
	* @param size the number of bytes to allow
	* @param no the index of the vector to allocate
	* @param requested_alignment the desired alignment (if relevant)
*/
void* personalized_malloc(size_t size, int no, int requested_alignment);

/**
	* Deallocator: frees a pointer that was returned by personalized_malloc (if relevant)
	* @param ptr the pointer to free
*/
void personalized_free(void* ptr);

/**
	* Destroyer: cleans the system off anything the initiator may have done
*/
void personalized_malloc_destroy();


#endif
