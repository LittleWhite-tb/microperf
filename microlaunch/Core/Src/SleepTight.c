#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include "Log.h"
#include "SleepTight.h"

static int should_sleep = 0;


static void sig_handler (int no)
{
	int old_errno = errno;

	sleep_tight_preparesignals ();

	switch (no)
	{
		case SIGNAL_SLEEP_TIGHT:
			should_sleep = 1;
			break;

		case SIGNAL_SLEEP_BUGBITE:
			should_sleep = 0;
			break;

		case SIGTERM:
			should_sleep = 2;
			break;
	}

	errno = old_errno;

}

void escape_from_scary_killers ()
{
	int res;
	FILE* file = fopen (PATH_TO_KILL, "r");

	res = file != NULL;

	if (res == 1 || should_sleep == 2)
	{
		if (res == 1) fclose(file);
		Log_output (-1, "Someone asked Microlauncher to stop => killing oneself.\n");
		exit (EXIT_FAILURE);
	}

}


int sleep_tight ()
{
	int res = 0;

	while (should_sleep == 1)
	{
		pause ();
		res = 1;
	}

	return res;
}

int sleep_tight_preparesignals ()
{
	int res;
	res = signal(SIGNAL_SLEEP_TIGHT, sig_handler) != SIG_ERR && signal(SIGNAL_SLEEP_BUGBITE, sig_handler) != SIG_ERR && signal (SIGTERM, sig_handler) == SIG_ERR;
	return res;
}
