/* signaltest.c - use of signals */
#include  <sys/types.h>		/* defines type pid_t */
#include  <errno.h>		/* defines errorcontrol variable errno */
#include  <signal.h>		/* defines signalnames */
#include  <stdio.h>		/* defines stderr, where errormessages are written */
#include  <stdlib.h>		/* defines exit() */
#include  <unistd.h>		/* defines sleep() */
#define   TRUE ( 1 )

pid_t childpid;			/* processid returned from fork */

void register_sighandler (int signal_code, void (*handler) (int sig))
{
  int return_value;		/* returnvalues from systemcalls */
  /* datastruktur for parametrar till systemanropet sigaction */
  struct sigaction signal_parameters;
  /*
   * ange parametrar for anrop till sigaction
   * sa_handler = den rutin som ska koras om signal ges
   * sa_mask     = mangd av ovriga signaler som ska sparras
   *               medan handlern kors (har: inga alls)
   * sa_flags    = extravillkor (har: inga alls)
   */
  signal_parameters.sa_handler = handler;
  sigemptyset (&signal_parameters.sa_mask);	/* skapar tom mangd */
  signal_parameters.sa_flags = 0;
  /* begar hantering av signal_code med ovanstaende parametrar */
  return_value = sigaction (signal_code, &signal_parameters, (void *) 0);
  if (-1 == return_value)	/* sigaction() misslyckades */
    {
      perror ("sigaction() failed");
      exit (1);
    }
}

					/* signal_handler skriver ut att processen fatt en signal.
					   Denna funktion registreras som handler for child-processen */
void signal_handler (int signal_code)
{
  char *signal_message = "UNKNOWN";	/* for signalnamnet */
  char *which_process = "UNKNOWN";	/* satts till Parent eller Child */
  if (SIGINT == signal_code)
    signal_message = "SIGINT";
  if (0 == childpid)
    which_process = "Child";
  if (childpid > 0)
    which_process = "Parent";
  fprintf (stderr, "%s process (pid %ld) caught signal no. %d (%s)\n",
	   which_process, (long int) getpid (), signal_code, signal_message);
}

								   /* cleanup_handler dodar child-processen vid SIGINT
								      Denna funktion registreras som handler for parent-processen */
void cleanup_handler (int signal_code)
{
  char *signal_message = "UNKNOWN";	/* for signalnamnet */
  char *which_process = "UNKNOWN";	/* satts till Parent eller Child */
  if (SIGINT == signal_code)
    signal_message = "SIGINT";
  if (0 == childpid)
    which_process = "Child";
  if (childpid > 0)
    which_process = "Parent";
  fprintf (stderr, "%s process (pid %ld) caught signal no. %d (%s)\n",
	   which_process, (long int) getpid (), signal_code, signal_message);
  /* if we are parent and signal was SIGINT, then kill child */
  if (childpid > 0 && SIGINT == signal_code)
    {
      int return_value;		/* for returvarden fran systemanrop */
      fprintf (stderr, "Parent (pid %ld) will now kill child (pid %ld)\n",
	       (long int) getpid (), (long int) childpid);
      return_value = kill (childpid, SIGKILL);	/* kill child process */
      if (-1 == return_value)	/* kill() misslyckades */
	{
	  perror ("kill() failed");
	  exit (1);
	}
      exit (0);			/* normal successful completion */
    }
}

int main ()
{
  fprintf (stderr, "Parent (pid %ld) started\n", (long int) getpid ());
  childpid = fork ();
  if (0 == childpid)
    {
      /* denna kod kors endast i child-processen */
      fprintf (stderr, "Child (pid %ld) started\n", (long int) getpid ());
      /* installera signalhanterare for SIGINT */
      register_sighandler (SIGINT, signal_handler);
      while (TRUE);		/* do nothing, just loop forever */
    }
  else
    {
      /* denna kod kors endast i parent-processen */
      if (-1 == childpid)	/* fork() misslyckades */
		{
		  perror ("fork() failed");
		  exit (1);
		}
      /*
         Kommer vi hit i koden sa ar vi i parent-processen
         och fork() har fungerat bra - i sa fall innehaller
         variabeln childpid child-processens process-ID
       */
      /*
       * Gor en liten paus innan vi registrerar parent-processens
       * signal-handler for SIGINT. Om vi trycker control-C i pausen
       * sa avbryts parent-processen och dor. Vad hander da med
       * child-processen? Prova!
       */
      sleep (1);
      register_sighandler (SIGINT, cleanup_handler);
      while (TRUE)
		{
		  int return_value;	/* for returvarden fran systemanrop */
		  fprintf (stderr,
			   "Parent (pid %ld) sending SIGINT to child (pid %ld)\n",
			   (long int) getpid (), (long int) childpid);
		  /* skicka SIGINT till child-processen, om och om igen */
		  return_value = kill (childpid, SIGINT);
		  if (-1 == return_value)	/* kill() misslyckades */
			{
			  fprintf (stderr,
				   "Parent (pid %ld) couldn't interrupt child (pid %ld)\n",
				   (long int) getpid (), (long int) childpid);
			  perror ("kill() failed");
			  /* programmet fortsatter aven om detta hander */
			}
		  sleep (1);		/* gor en liten paus mellan varje SIGINT --
					   vi ska inte stress-testa systemet */
		}
    }
}
