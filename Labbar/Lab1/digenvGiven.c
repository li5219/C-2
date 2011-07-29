#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

pid_t child_pid;		/* Gets child PID when doing fork() */

int main (int argc, char argv)
{
  int return_value;		/* Gets return values upon systemcalls */
  int pipe_filedesc[2];		/* Gets filedescriptors from pipe */

  fprintf (stderr, "Parent (producer, pid %ld) started\n",
	   (long int) getpid ());

  return_value = pipe (pipe_filedesc);	/* Create a pipe */
  if (-1 == return_value)	/* If pipe fails, exit program */
    {
      perror ("Cannot create pipe");
      exit (1);
    }

  child_pid = fork ();		/* Create child process */

  if (0 == child_pid)
    {

      string parameter;

      fprintf (stderr, "Child (consumer, pid %ld) started\n",
	       (long int) getpid ());

      /* Close readside of the pipe */
      return_value = close (pipe_filedesc[1]);
      if (-1 == return_value)
	{
	  perror ("Cannot close pipe");
	  exit (1);
	}

    }
}
