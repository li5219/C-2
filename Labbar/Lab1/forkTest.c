#include <sys/types.h>		/* definierar bland annat typen pid_t */
#include <errno.h>		/* definierar felkontrollvariabeln errno */
#include <stdio.h>		/* definierar stderr, dit felmeddelanden skrivs */
#include <stdlib.h>		/* definierar bland annat exit() */
#include <unistd.h>		/* definierar bland annat fork() */

pid_t childpid;			/* processid som returneras från fork */

int main ()
{
	childpid = fork ();
	if (0 == childpid)
    {
		/* This code only runs in child-process */
		printf("Child\t%d %d\n", getpid(), getppid());
    }
	else
	{
		/* This code only runs in parent-process */
		if (-1 == childpid)	/* fork() failed */
			{
				char *errormessage = "UNKNOWN";	/* Errormessage */
				if (EAGAIN == errno)
					errormessage = "Can not allocate page table";
				if (ENOMEM == errno)
					errormessage = "Can not allocate kernel data";
				fprintf (stderr, "fork() failed because: %s\n", errormessage);
				exit (1);		/* Program is terminated with error number 1 */
			}
		/*
		* Code in here is run in the parent process and
		* thus the fork() worked. chilpid contains the ID
		* of the child process.
		*/
		printf("Parent\t%d %d\n", getpid(), childpid);
	}

	exit (0);			/* programmet avslutas normalt */
}
