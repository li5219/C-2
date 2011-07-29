/* duptest.c - use of dup2() for redirection */
#include <sys/types.h>		/* define type pid_t */
#include <sys/wait.h>		/* define WIFEXITED */
#include <errno.h>		/* define errno */
#include <stdio.h>		/* define stderr */
#include <stdlib.h>		/* define rand() and RAND_MAX */
#include <unistd.h>		/* define pipe() and STDIN_FILENO */
#define PIPE_READ_SIDE ( 0 )
#define PIPE_WRITE_SIDE ( 1 )

pid_t childpid;			/* för child-processens PID vid fork() */

int main ()
{
  int pipe_filedesc[2];		/* för fildeskriptorer från pipe(2) */
  int return_value;		/* för returvärden från systemanrop */
  int status;			/* för returvärden från child-processer */
  fprintf (stderr, "Starting command: ls | wc\n");
  return_value = pipe (pipe_filedesc);	/* skapa en pipe */
  if (-1 == return_value)	/* om pipe() misslyckades */
    {
      perror ("Cannot create pipe");
      exit (1);
    }
  childpid = fork ();		/* skapa första child-processen (för ls) */
  if (0 == childpid)
    {
      /* denna kod körs endast i child-processen (för ls) */
      /* ersätt stdout med duplicerad skriv-ände på pipen */
      return_value = dup2 (pipe_filedesc[PIPE_WRITE_SIDE], STDOUT_FILENO);	/* STDOUT_FILENO == 1 */
      if (-1 == return_value)
	    {
	      perror ("Cannot dup");
	      exit (1);
	    }
      /* ls ska inte läsa från pipen -- stäng läs-änden */
      return_value = close (pipe_filedesc[PIPE_READ_SIDE]);
      if (-1 == return_value)
	    {
	      perror ("Cannot close read end");
	      exit (1);
	    }
      /* ls ska skriva till pipen. Men bara med fildeskriptor 1,
         det vill säga den som stdout hade före anropet till dup2().
         Alltså ska vi också stänga skriv-änden på pipen.
         Obs, vi tappar inte kontakten med pipen eftersom vi har kvar
         en fildeskriptor till den, på stdouts gamla plats. */
      return_value = close (pipe_filedesc[PIPE_WRITE_SIDE]);
      if (-1 == return_value)
	    {
	      perror ("Cannot close write end");
	      exit (1);
	    }
      (void) execlp ("ls", "ls", (char *) 0);
      /* exec returnerar bara om något fel har uppstått
         och om exec returnerar så är returvärdet alltid -1 */
      perror ("Cannot exec ls");
      exit (1);
      /* slut på kod för första child-processen (för ls) */
    }
  /* Denna kod körs endast i parent-processen. Det behövs inget "else"
     eftersom child-processen aldrig kan nå slutet på if-satsen.
     Fungerar child-processen bra så byter den ut detta program mot ls.
     Fungerar child-processen dåligt så avslutas den med exit( 1 ). */
  if (-1 == childpid)		/* fork() misslyckades */
    {
      perror ("Cannot fork()");
      exit (1);
    }
  /* Kommer vi hit i koden så är vi i parent-processen
     och fork() har fungerat bra. */
  childpid = fork ();		/* skapa andra child-processen (för wc) */
  if (0 == childpid)
    {
      /* denna kod körs endast i child-processen (för wc) */
      /* ersätt stdin med duplicerad läs-ände på pipen */
      return_value = dup2 (pipe_filedesc[PIPE_READ_SIDE], STDIN_FILENO);	/* STDIN_FILENO == 0 */
      if (-1 == return_value)
	    {
	      perror ("Cannot dup");
	      exit (1);
	    }
      /* wc ska inte skriva till pipen -- stäng skriv-änden */
      return_value = close (pipe_filedesc[PIPE_WRITE_SIDE]);
      if (-1 == return_value)
	    {
	      perror ("Cannot close read end");
	      exit (1);
	    }
      /* wc ska läsa från pipen. Men bara med fildeskriptor 0,
         det vill säga den som stdin hade före anropet till dup2().
         Alltså ska vi också stänga läs-änden på pipen.
         Obs, vi tappar inte kontakten med pipen eftersom vi har kvar
         en fildeskriptor till den, på stdins gamla plats. */
      return_value = close (pipe_filedesc[PIPE_READ_SIDE]);
      if (-1 == return_value)
	    {
	      perror ("Cannot close write end");
	      exit (1);
	    }
      (void) execlp ("wc", "wc", (char *) 0);
      /* exec returnerar bara om något fel har uppstått
         och om exec returnerar så är returvärdet alltid -1 */
      perror ("Cannot exec wc");
      exit (1);
      /* slut på kod för andra child-processen (för wc) */
    }
  /* Denna kod körs endast i parent-processen */
  if (-1 == childpid)		/* fork() misslyckades */
    {
      perror ("Cannot fork()");
      exit (1);
    }
  /* Kommer vi hit i koden så är vi i parent-processen
     och fork() har återigen fungerat bra. */
  /* Parent-processen ska inte använda pipen -- stäng båda ändar */
  return_value = close (pipe_filedesc[PIPE_READ_SIDE]);
  if (-1 == return_value)
    {
      perror ("Cannot close read end");
      exit (1);
    }
  return_value = close (pipe_filedesc[PIPE_WRITE_SIDE]);
  if (-1 == return_value)
    {
      perror ("Cannot close write end");
      exit (1);
    }
  childpid = wait (&status);	/* Vänta på ena child-processen */
  if (-1 == childpid)
    {
      perror ("wait() failed unexpectedly");
      exit (1);
    }
  if (WIFEXITED (status))	/* child-processen har kört klart */
    {
      int child_status = WEXITSTATUS (status);
      if (0 != child_status)	/* child had problems */
	    {
	      fprintf (stderr, "Child (pid %ld) failed with exit code %d\n", (long int) childpid, child_status);
	    }
    }
  else
    {
      if (WIFSIGNALED (status))	/* child-processen avbröts av signal */
		{
		  int child_signal = WTERMSIG (status);
		  fprintf (stderr, "Child (pid %ld) was terminated by signal no. %d\n", (long int) childpid, child_signal);
		}
    }
  childpid = wait (&status);	/* Vänta på andra child-processen */
  if (-1 == childpid)
    {
      perror ("wait() failed unexpectedly");
      exit (1);
    }
  if (WIFEXITED (status))	/* child-processen har kört klart */
    {
      int child_status = WEXITSTATUS (status);
      if (0 != child_status)	/* child had problems */
		{
		  fprintf (stderr, "Child (pid %ld) failed with exit code %d\n", (long int) childpid, child_status);
		}
    }
  else
    {
      if (WIFSIGNALED (status))	/* child-processen avbröts av signal */
		{
		  int child_signal = WTERMSIG (status);
		  fprintf (stderr, "Child (pid %ld) was terminated by signal no. %d\n", (long int) childpid, child_signal);
		}
    }
  exit (0);			/* Avsluta parent-processen på normalt sätt */
}
