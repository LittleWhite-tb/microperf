#include "sa_malloc.h"

int allocationInit (void* ptr)
{
	(void) ptr;
	return 0;
}

void *allocationMalloc (size_t size, int no, int requested_alignment)
{
	(void) no;
	return (sa_malloc (size, requested_alignment));
}

void allocationFree (void* ptr)
{
	sa_free (ptr);
}

void allocationClose ()
{
	return;
}
