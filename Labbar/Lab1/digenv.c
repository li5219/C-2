#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READSIDE ( 0 )
#define WRITESIDE ( 1 )

pid_t childpid;			/* Values for child-process id upon fork() */

void wait_child(void)
{
    pid_t cpid;
    int st, cstat;

    /* Wait for child */
	if(-1 == (cpid = wait(&st)))
	{
		perror("** An error occured in \"wait\" waiting for a child");
		exit(1);
	}

	if(WIFEXITED(st))
	{
		/* Check exit status of child */
		cstat = WEXITSTATUS(st);
		if(0 != cstat)
		{
			fprintf(stderr, "** Child with PID %ld failed with exit code: %d.\n", (long) cpid, cstat);
		}
	}
	else if(WIFSIGNALED(st))
	{
		/* Check what signal terminated the child */
		fprintf(stderr, "** Child with PID %ld was terminated by signal: %d.\n", (long) cpid, WTERMSIG(st));
	}
}

void closePipes(int fd1[], int fd2[], int fd3[])
  {
    if( -1 == close(fd1[0]))
    	fprintf(stderr, "Error closing readside of printenv");
    if( -1 == close(fd1[1]))
    	fprintf(stderr, "Error closing writeside of printenv");
    if( -1 == close(fd2[0]))
    	fprintf(stderr, "Error closing readside of grep");
    if( -1 == close(fd2[1]))
    	fprintf(stderr, "Error closing writeside of grep");
    if( -1 == close(fd3[0]))
    	fprintf(stderr, "Error closing readside of sort");
    if( -1 == close(fd3[1]))
    	fprintf(stderr, "Error closing writeside of sort");
  }
  
char* choosePager()
{
  char *pager, *test;
  test = getenv("PAGER");
  if(test == NULL)
    pager = "less";
  else
    pager = test;

  return pager;
}
  
int main (int argc, char **argv)
{
  int printenvFD[2], grepFD[2], sortFD[2];  /* Filedescriptors for the pipes*/
  int ctrlValues;  /* Values from system calls */
  int useGrep=0;  /* Value to use for checking the use of grep function */
  char *args[] = {"", NULL};  /* Used in the calling of exec commands*/
  char *pager = choosePager();

  ctrlValues = pipe(printenvFD);
  if (-1 == ctrlValues)	/* pipe() failed */
    {
      perror ("Cannot create pipe\n");
      exit (1);
    }
  ctrlValues = pipe(grepFD);
  if (-1 == ctrlValues)	/* pipe() failed */
    {
      perror ("Cannot create pipe\n");
      exit (1);
    }
  ctrlValues = pipe(sortFD);
  if (-1 == ctrlValues)	/* pipe() failed */
    {
      perror ("Cannot create pipe\n");
      exit (1);
    }

  /* A check to determine whether to use grep or not.
   * This depending on if there were any arguments to
   * the program or not. */
  useGrep = (argc > 1 ? 1 : 0);
  
  childpid = fork();
  if (childpid == 0) /* Child process for printenv */
    {
      ctrlValues = dup2 (printenvFD[WRITESIDE], STDOUT_FILENO);	/* Duplicate filedescriptor and replace stdout */
	  if (-1 == ctrlValues)
		{
		  perror ("Cannot duplicate filedescriptors for printenv\n");
		  exit (1);
		}
	  closePipes(printenvFD, grepFD, sortFD);
	  
      execvp ("printenv", args);
      fprintf(stderr, "Error in 'printenv'\n");
    }
  if (-1 == childpid)
    {
      fprintf(stderr, "Cannot fork for printenv\n");
      exit(1);
    } /* printenv */

  /*
   * How many arguments did we get upon calling program?
   * If more than 1, then we will throw them to >grep< */
  if(useGrep)
    {
      childpid = fork ();
      if (childpid == 0) /* Child process for grep */
        {
          ctrlValues = dup2 (printenvFD[READSIDE], STDIN_FILENO);
          if (-1 == ctrlValues)
            {
              perror ("Cannot duplicate filedescriptors for printenv\n");
              exit (1);
            }
          ctrlValues = dup2 (grepFD[WRITESIDE], STDOUT_FILENO);
          if (-1 == ctrlValues)
            {
              perror ("Cannot duplicate filedescriptors for grep\n");
              exit (1);
            }
          closePipes(printenvFD, grepFD, sortFD);
          
          execvp ("grep", argv);
          fprintf(stderr, "Error in 'grep'\n");
        }
      if (-1 == childpid)
        {
          fprintf(stderr, "Cannot fork for grep\n");
          exit(1);
        }
    } /* grep */

  childpid = fork ();
  if (childpid == 0) /* Child process for sort */
    {
	  if(useGrep)
		{
		  ctrlValues = dup2 (grepFD[READSIDE], STDIN_FILENO);
		  if (-1 == ctrlValues)
			{
			  perror ("Cannot duplicate filedescriptor for grep\n");
			  exit (1);
			}
		  ctrlValues = dup2 (sortFD[WRITESIDE], STDOUT_FILENO);
		  if (-1 == ctrlValues)
			{
			  perror ("Cannot duplicate filedescriptor for sort\n");
			  exit (1);
			}
		} else {
		  ctrlValues = dup2 (printenvFD[READSIDE], STDIN_FILENO);
		  if (-1 == ctrlValues)
			{
			  perror ("Cannot duplicate filedescriptors for printenv\n");
			  exit (1);
			}
		  ctrlValues = dup2 (sortFD[WRITESIDE], STDOUT_FILENO);
		  if (-1 == ctrlValues)
			{
			  perror ("Cannot duplicate filedescriptor for sort\n");
			  exit (1);
          }
        }
      closePipes(printenvFD, grepFD, sortFD);

      execvp ("sort", args);
      fprintf(stderr, "Error in 'sort'\n");
    }
  if (-1 == childpid)
    {
      fprintf(stderr, "Cannot fork for sort\n");
      exit(1);
    } /* sort */

  childpid = fork ();
  if (childpid == 0) /* Child process for pager */
    {
      ctrlValues = dup2 (sortFD[READSIDE], STDIN_FILENO);
      if (-1 == ctrlValues)
        {
          perror ("Cannot duplicate filedescriptors for sort\n");
          exit (1);
        }
      closePipes(printenvFD, grepFD, sortFD);

      execvp (pager, args);
      if(ENOENT == errno)
        {
          fprintf(stderr, "Pager not found, using 'less'");
          execvp ("less", args);
		  if(ENOENT == errno)
			{
			  fprintf(stderr, "Pager not found, using 'more'");
			  execvp ("more", args);
			}
        }
      fprintf(stderr, "Error in 'less'\n");
    }
  if (-1 == childpid)
    {
      fprintf(stderr, "Cannot fork for less\n");
      exit(1);
    } /* pager */

  closePipes(printenvFD, grepFD, sortFD);

  wait_child();
  wait_child();
  wait_child();
  if(useGrep)
    wait_child();

  return 0;
}
