#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READSIDE ( 0 )
#define WRITESIDE ( 1 )

pid_t childpid;			/* Values for child-process id upon fork() */

void wait_child( void )
{
	pid_t cpid;
	int st, cstat;

	/* Wait for child */
	if( -1 == ( cpid = wait( &st ) ) )
	{
		perror( "** An error occured in \"wait\" waiting for a child" );
		exit( 1 );
	}

	if( WIFEXITED( st ) )
	{
		/* Check exit status of child */
		cstat = WEXITSTATUS( st );
		if( 0 != cstat )
		{
			fprintf( stderr, "** Child with PID %ld failed with exit code: %d.\n",
				(long) cpid, cstat );
		}
	}
	else if( WIFSIGNALED( st ) )
	{
		/* Check what signal terminated the child */
		fprintf( stderr, "** Child with PID %ld was terminated by signal: %d.\n",
			(long) cpid, WTERMSIG( st ) );
	}
}

int main (int argc, char **argv)
{
  int printenvFD[2];
  int grepFD[2];
  int sortFD[2];
  int ctrlValues;		/* Values from system calls */
  int useGrep=0;
  char *args[] = {"", NULL};

  ctrlValues = pipe(printenvFD);
  ctrlValues = pipe(grepFD);
  ctrlValues = pipe(sortFD);
  if (-1 == ctrlValues)	/* pipe() failed */
    {
      perror ("Cannot create pipe\n");
      exit (1);
    }

  useGrep = (argc > 1 ? 1 : 0);
  
  childpid = fork();
  if (childpid == 0) /* printenv */
    {
      ctrlValues = dup2 (printenvFD[WRITESIDE], STDOUT_FILENO);	/* Duplicate filedescriptor and replace stdout */
	  if (-1 == ctrlValues)
		{
		  perror ("Cannot dup2 printenv\n");
		  exit (1);
		}
      ctrlValues = close (printenvFD[WRITESIDE]);
	  if (-1 == ctrlValues)
		{
		  perror ("Cannot close READSIDE printenv\n");
		  exit (1);
		}
      execvp ("printenv", args);
      fprintf(stderr, "Error in 'printenv'\n");
    }
  if (-1 == childpid)
    {
      fprintf(stderr, "Cannot fork for printenv\n");
      exit(1);
    }

  if(useGrep)
    {
      childpid = fork ();
      if (childpid == 0) /* grep */
        {
		  ctrlValues = dup2 (printenvFD[READSIDE], STDIN_FILENO);
          if (-1 == ctrlValues)
            {
              perror ("Cannot dup2 printenv less\n");
              exit (1);
            }
          ctrlValues = close (printenvFD[WRITESIDE]);
          if (-1 == ctrlValues)
            {
              perror ("Cannot close WRITESIDE less\n");
              exit (1);
            }
          execvp ("grep", argv);
          fprintf(stderr, "Error in 'less'\n");
        }
      if (-1 == childpid)
        {
          fprintf(stderr, "Cannot fork for less\n");
          exit(1);
        }
    } /* End Grep */
    else {
      childpid = fork ();
      if (childpid == 0) /* grep */
        {
		  ctrlValues = dup2 (printenvFD[READSIDE], STDIN_FILENO);
          if (-1 == ctrlValues)
            {
              perror ("Cannot dup2 printenv less\n");
              exit (1);
            }
          ctrlValues = close (printenvFD[WRITESIDE]);
          if (-1 == ctrlValues)
            {
              perror ("Cannot close WRITESIDE less\n");
              exit (1);
            }
            char *ar[] = {"", NULL};
          execvp ("sort", ar);
          fprintf(stderr, "Error in 'less'\n");
        }
      if (-1 == childpid)
        {
          fprintf(stderr, "Cannot fork for less\n");
          exit(1);
        }
    	}

    close(printenvFD[0]);
    close(printenvFD[1]);
    close(sortFD[0]);
    close(sortFD[1]);

	wait_child();
	if(useGrep)
	  wait_child();

	return 0;
}
