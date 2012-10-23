#include <stdio.h>
#include <stdlib.h>
#include "Log.h"
#include "Progress.h"

void Benchmark_printProgress (int isPrintingProcess, unsigned current_step, unsigned max, int overhead, int newline)
{
	if (isPrintingProcess)
	{
		if (overhead)
		{
			fprintf (stderr, "\r - Overhead computation process : %3d%% (%d/%d)", (current_step * 100) / (max), current_step, max);
		}
		else
		{
			fprintf (stderr, "\r - Benchmark computation process : %3d%% (%d/%d)", (current_step * 100) / (max), current_step, max);
		}
		
		if (newline)
		{
			fprintf (stderr, "\n");
		}
	}
}

void Benchmark_printDataSavingProgress (int isPrintingProcess, unsigned current_step, unsigned max, unsigned times, unsigned offset, int newline)
{
	if (isPrintingProcess)
	{
		fprintf (stderr, "\r - Data saving... %3d%%", (current_step*times)/max + offset);
		
		if (newline)
		{
			fprintf (stderr, "\n");
		}
	}
}

void Benchmark_printLaunchingTechnique (int isPrintingProcess, int technique)
{
	char buf[512];
	if (isPrintingProcess)
	{
		switch (technique)
		{
			case KERNEL_MODE:
				snprintf (buf, 512, "KERNEL MODE");
				break;
			case EXEC_MODE:
				snprintf (buf, 512, "EXEC MODE");
				break;
			default:
				Log_output (-1, "Error: Unknown launching technique\n");
				exit (EXIT_FAILURE);
				break;
		}
		
		Log_output (-1, "===================================\n");
		Log_output (-1, "LAUNCHING BENCHMARK (%s)\n", buf);
		Log_output (-1, "===================================\n");
	}
}
