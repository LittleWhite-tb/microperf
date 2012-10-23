#include <stdlib.h>

int allocationInit (void* ptr)
{
	(void) ptr;
	return 0;
}

void* allocationMalloc (size_t size, int no, int requested_alignment)
{
	(void) no;
	(void) requested_alignment;
	return(malloc(size));
}

void allocationFree (void* ptr)
{
	free(ptr);
}

void allocationClose ()
{
	return;
}
