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

#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>

static int core = 0;

int getNbProcessorsAvailable ( void )
{
	FILE *output;
	long nbProcessorsAvailable;
	char buf[3]; /* Max processors = 999 */
	char cmd[] = "grep \"CPU\" /proc/cpuinfo | wc -l";
	int status;
	char *ptr, *end;
	
	output = popen (cmd, "r");
	
	if (output == NULL)
	{
		fprintf (stderr, "%s\n", cmd);
		perror ("Error: Getting processors number : ");
		exit (EXIT_FAILURE);
	}
	
    ptr = fgets (buf, sizeof (buf), output);
    assert(ptr != NULL);
    
    status = pclose(output);
    
    if (status == -1)
    {
		fprintf (stderr, "%s\n", buf);
    	perror ("Error: Closing processors number system call :");
    	exit (EXIT_FAILURE);
    }
    
    nbProcessorsAvailable = strtol (buf, &end, 10);
    if (buf == end)
    {
    	fprintf (stderr, "Error: in getNbProcessorsAvailable strtol call failed.\n");
    	exit (EXIT_FAILURE);
    }
    
    return nbProcessorsAvailable;
}

int pthread_create ( pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg ) {
	int (*real_pthread_create) (pthread_t*, const pthread_attr_t*, void *(*routine)(void *), void*) = dlsym (RTLD_NEXT, "pthread_create");
	int (*real_pthread_setaffinity_np) (pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset) = dlsym (RTLD_NEXT, "pthread_setaffinity_np");
	int res;
	cpu_set_t cpuset;
	int nbProcessors = getNbProcessorsAvailable ();
	
	assert (real_pthread_create != NULL);
	assert (real_pthread_setaffinity_np != NULL);
	res = real_pthread_create (thread, attr, start_routine, arg);
	
	/* Pinning stuff */
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);
	
	if (real_pthread_setaffinity_np (*thread, sizeof (cpu_set_t), &cpuset) != 0) {
		fprintf (stderr, "Error: Cannot pin thread %p on core %d\n", thread, core);
		perror ("");
	}
	
	core++;
	core %= nbProcessors;
	
	return res;
}
