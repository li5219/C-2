/* pipetest.c - using pipe */
#include <sys/types.h>		/* define type pid_t */
#include <errno.h>		/* define perror() */
#include <stdio.h>		/* define stderr */
#include <stdlib.h>		/* define rand() and RAND_MAX */
#include <unistd.h>		/* define pipe() */
#define TRUE ( 1 )		/* logical constant TRUE */
#define MINNUM (    999 )	/* smallest possible value generated */
#define MAXNUM ( 99999 )	/* largest possible value generated */
pid_t child_pid;		/* PID for child after fork() */

int main ()
{
  int return_value;		/* return values from system calls */
  int pipe_filedesc[2];		/* filedescriptors from pipe() */
  fprintf (stderr, "Parent (producer, pid %ld) started\n", (long int) getpid ());
  return_value = pipe (pipe_filedesc);	/* create a pipe */
  if (-1 == return_value)	/* quit program is pipe fails */
    {
      perror ("Cannot create pipe");
      exit (1);
    }
  child_pid = fork ();		/* create child-process */
  if (0 == child_pid)
    {
      /* this code only runs in child process */
      int tal;			/* gets numbers from parent */
      fprintf (stderr, "Child (consumer, pid %ld) started\n", (long int) getpid ());
      /* close write side of pipe - consumer will only read*/
      return_value = close (pipe_filedesc[1]);
      if (-1 == return_value)
		{
		  perror ("Cannot close pipe");
		  exit (1);
		}
      while (1)
		{
		  return_value = read (pipe_filedesc[0], &tal, sizeof (tal));
		  if (-1 == return_value)	/* read() failed */
			{
			  perror ("Cannot read from pipe");
			  exit (1);
			}
		  if (0 == return_value)	/* end of file */
			{
			  fprintf (stderr, "End of pipe, child exiting\n");
			  exit (0);
			}
		  if (sizeof (tal) == return_value)
			{
			  /* divide the number in primes and print */
			  int f = 2, p = 0, inc[11] = { 1, 2, 2, 4, 2, 4, 2, 4, 6, 2, 6 };
			  printf ("Consumer mottog talet %d, primfaktorer ", tal);
			  while (f * f <= tal)
			if (0 != tal % f)
			  {
				f = f + inc[p++];
				if (p > 10)
				  p = 3;
			  }
			else
			  {
				printf ("%d ", f);
				tal = tal / f;
			  };
			  printf ("%d\n", tal);
			}
		  else
			{
			  /* Unexpected byte count -- inform user and continue */
			  fprintf (stderr,
				   "Child wanted %d bytes but read only %d bytes\n",
				   sizeof (tal), return_value);
			}
		}
    }
  else
    {
      /* this code only runs in parent */
      if (-1 == child_pid)	/* fork() failed */
		{
		  perror ("fork() failed");
		  exit (1);
		}
      /*
       * if we get this far, we are in parent process
       * and fork() finished without error
       */
      /* close read end of pipe - parent will only write */
      return_value = close (pipe_filedesc[0]);
      if (-1 == return_value)
		{
		  perror ("Parent cannot close read end");
		  exit (1);
		}
      while (TRUE)
		{
		  /* slumpa fram nya lagom stora tal, om och om igen */
		  /* " + 0.0 " tvingar fram flyttalsberäkning i stället för heltal */
		  double tmp = (rand () + 0.0) / (RAND_MAX + 0.0);
		  /* tmp har nu ett värde mellan 0 och 1 */
		  int generated_number = MINNUM + tmp * (MAXNUM - MINNUM + 0.0);
		  printf ("Producer skapade talet %d\n", generated_number);
		  return_value = write (pipe_filedesc[1],
					&generated_number, sizeof (generated_number));
		  if (-1 == return_value)
			{
			  perror ("Cannot write to pipe");
			  exit (1);
			}
		  sleep (1);		/* ta det lugnt en stund */
		}
    }
}
