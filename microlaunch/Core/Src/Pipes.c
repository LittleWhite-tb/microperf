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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Pipes.h"
#include "SleepTight.h"

void Pipe_generate ( SPipe *p )
{
	if (pipe (p->sf) == -1)
	{
		perror ("Error: pipe->sf creation");
		exit (EXIT_FAILURE);
	}

	if (pipe (p->fs) == -1) {
		perror ("Error: pipes->fs creation");
		exit (EXIT_FAILURE);
	}
}

int Pipe_closeChildUnusedSide ( SPipe *p )
{
	int res = 0;
	res |= close (p->sf[0]);
	res |= close (p->fs[1]);
	
	return res;
}

int Pipe_closeChildUsedSide ( SPipe *p )
{
	int res = 0;
	res |= close (p->sf[1]);
	res |= close (p->fs[0]);
	
	return res;
}

int Pipe_closeFatherUnusedSide ( SPipe *p )
{
	int res = 0;
	res |= close (p->sf[1]);
	res |= close (p->fs[0]);
	
	return res;
}

int Pipe_closeFatherUsedSide ( SPipe *p )
{
	int res = 0;
	res |= close (p->sf[0]);
	res |= close (p->fs[1]);
	
	return res;
}

ssize_t Pipe_childRead ( int pipeFS[2], int *res )
{
	int tmp;
	int *r;
	ssize_t size;
	assert ( pipeFS != NULL);
	
	if (res == NULL)
	{
		r = &tmp;
	}
	else
	{
		r = res;
	}
	
	size = read (pipeFS[0], &r, sizeof(r));
	if (size < 1)
	{
		// If the file PATH_TO_KILL exists, cancel the program (SleepTight.h)
		escape_from_scary_killers();
		perror ("Child: Error when reading from pipeFS[0]");
		exit (EXIT_FAILURE);
	}
	
	return size;
}

ssize_t Pipe_childWrite ( int pipeSF[2], int value )
{
	ssize_t size;
	assert( pipeSF != NULL);
	size = write (pipeSF[1], &value, sizeof (value));
	
	if	(size < 1)
	{
		// If the file PATH_TO_KILL exists, cancel the program (SleepTight.h)
		escape_from_scary_killers();
		perror ("Child: Error when writing in pipeSF[1]");
		exit (EXIT_FAILURE);
	}
	
	return size;
}

ssize_t Pipe_fatherRead ( int pipeSF[2], int *res )
{
	int tmp;
	int *r;
	ssize_t size = 0;
	assert ( pipeSF != NULL);
	
	if (res == NULL)
	{
		r = &tmp;
	}
	else
	{
		r = res;
	}

	while(size == 0)
	{

		size = read (pipeSF[0], &r, sizeof(r));
		if (size < 1)
		{
			if(errno == EINTR)
			{
				size = 0;
			}
			else
			{
				// If the file PATH_TO_KILL exists, cancel the program (SleepTight.h)
				escape_from_scary_killers();
				perror ("Father: Error when reading from pipeSF[0]");
				exit (EXIT_FAILURE);
			}
		}

	}
	
	return size;
}

ssize_t Pipe_fatherWrite ( int pipeFS[2], int value )
{
	ssize_t size = 0;
	assert( pipeFS != NULL);

	while(size == 0)
	{
		size = write (pipeFS[1], &value, sizeof (value));
	
		if(size < 1)
		{
			if(errno == EINTR)
			{
				size = 0;
			}
			else
			{
				// If the file PATH_TO_KILL exists, cancel the program (SleepTight.h)
				escape_from_scary_killers();
				perror ("Father: Error when writing in pipeFS[1]");
				exit (EXIT_FAILURE);
			}
		}
	}
	
	return size;
}
