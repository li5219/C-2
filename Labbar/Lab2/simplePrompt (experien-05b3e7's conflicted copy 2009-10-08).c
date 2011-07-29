#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#define allocated 70
#define TRUE	1
#define	FALSE	0
#ifdef SH
        #define waiter(signal) handlInt(signal)
#endif
#ifndef SH
        #define waiter(signal) waitFunc(signal)
#endif

int checkIfEmpty (char inc[]);
char *parseEnd (char *inc);
pid_t id;

/* Function to capture Ctrl+C */ 
void sighandler (int sigNum)
{
        fprintf (stderr, "Catched signal: %d\n", sigNum);
        if (sigNum == 2)
                kill(getpid(), SIGTERM);
}

void waitFunc (int signal) 
{
	int status;
	if (signal == 0)
	{
		waitpid (-1, &status, WNOHANG);
	}
	else if (signal == 1)
	{
		waitpid (-1, &status, 0);
	}
}

void handlInt (int sig)
{
/*	signal(SIGCHLD, sighandler);
	if(sig)
	{
		fprintf(stderr, "Killing %d\n", id);
		kill(id, SIGINT);
	} else {
		fprintf(stderr, "\nKilling shell\n");
		exit(0);
	}
*/}

/*
Function to wait for child process and print the exit status
*/
void waitChild () 
{
	pid_t cpid;
	int status, cstat;
  	if (-1 == (cpid = wait (&status)))
  	{
		fprintf (stderr, "Error while waiting for child (%d).\n", cpid);
	}
  	if (WIFEXITED (status))
	{
		cstat = WEXITSTATUS (status);
		if (0 != cstat)
			fprintf (stderr, "Child with PID %ld failed with error: %d\n", (long) cpid, cstat);
                else
                        fprintf (stderr, "Child with PID %ld terminated normaly.\n", (long) cpid);
	}
	else if (WIFSIGNALED (status))
	{
		fprintf (stderr, "Child with PID %ld terminated with signal: %d\n", (long) cpid, WTERMSIG (status));
        }
}

/*
Function to tokenize the command string
*/
char **parseString (char *incoming) 
{
	int empty, i = 0, len = strlen (incoming) - 1;
	char **result = NULL, *ret, delims[] = " ", *str = NULL;
	
	result = malloc (len);
	empty = checkIfEmpty (incoming);	/*Checking if it's only an enter without any command */
  	if (empty == FALSE)
  	{
		ret = malloc (len);
		ret = parseEnd (incoming);
		str = strtok (ret, delims);

                while( str != NULL )
                {
                        result[i] = str;
                        i = i + 1;
                        str = strtok( NULL, delims );
                }
		return result;
	} else {
		*result = "empty";
		return result;
	}
}

int checkIfEmpty (char *incoming) 
{
	if (strlen (incoming) > 1)
		return FALSE;
	else
		return TRUE;
}
/*
Used to remove the trailing \0
* char *inc: 
*/
char *parseEnd (char *inc) 
{
	int i, len = strlen (inc) - 1;
	char *ret = malloc (len);

	for (i = 0; i < len && inc[i] != '\0'; i++)
	{
		ret[i] = inc[i];
	}
	return ret;
}

/*
* char *inc: String containing data to be checked
*/
int lastElement (char *inc[]) 
{
	int i = 0;

	while (inc[i] != NULL)
	{
		i = i + 1;
	}
	i = i - 1;
	return i;
}

/*
Simple function to change the directory on the command 'cd'. If the directory does not exist, the default directory will be user home.
* char *dir: String containing path to directory to change to
* int returnValue: Used to get returnvalue of the command 'chdir' so it can be checked for errors
*/
void changeDirectory (char *dir)
{
	int returnValue;
	char *home = malloc(strlen(getenv("HOME")));
	home = getenv("HOME");
	
	returnValue = chdir (dir);
	if (returnValue == -1)
	{
		fprintf (stderr, "Error: %s\n Directory does not exist. Changing to user home directory\n", strerror (errno));
		if (-1 == chdir (home))
                        fprintf (stderr, "Error: %s\nHome directory not valid", strerror (errno));
	}
}

/*
Function to execute the commands given.
Input:
* char **printed: An array of the command and arguments from the user
* pid_t parent: Process ID of parent process
*/
void doCommand (char **printed, pid_t parent)
{
	fprintf (stderr, "Parent (%d) created child (%d)\n", parent, getpid ());
	execvp (printed[0], printed);
	fprintf (stderr, "**** Command not found '%s' ****\n", printed[0]);
	exit (1);

	fprintf (stderr, "Parent (%d) created child (%d)\n", parent, getpid ());
}
/*
Simply prints out if errors occured while trying to fork a new process
*/
void printError ()
{
        char *errmess = "Unknown error.\n";
	if (EAGAIN == errno)
		errmess = "Can not allocate page table.\n";
        if (ENOMEM == errno)
		errmess = "Can not allocate kernel data.\n";
        fprintf (stderr, "fork() failed with error: %s\n", errmess);
	exit (1);
}

/*
* size_t size: Used for allocation of the absolute pathname of current directory
* int fg: Helps when calling waiter functions. Is 1 or 0 if the process is a foreground or background respectively
* char *buffer: Array containing the pathname of current directory
* char **printed: Tokenized commands from user
* char *command: The string of commands given by the user
* char *cd : Strings used for simple comparing of commands. The uses are selfexplainatory
       *quit
       *background
       *empty
* pid_t childProcess, parent: Contains processid's of parent and child
* timeval start, end: Used to get the time of the process execution
*/
int main (void)
{
	size_t size = 100;
	int fg;
	char *buffer = (char *) malloc (size), **printed, *command = malloc (allocated);
	char *cd = "cd", *quit = "exit", *background = "&", *empty = "empty";
	pid_t childProcess, parent = getpid ();
  	struct timeval start, end;

	while (TRUE)
	{
	        fprintf (stderr, "%s @ %s: ", getenv ("USER"), getcwd (buffer, size));
		fgets (command, allocated, stdin);
		printed = parseString (command);
		#ifdef SH
			signal (SIGINT, sighandler);
		#endif
		if (strcmp (*printed, empty) != 0)
		{
			if ((strcmp (printed[0], quit)) == 0)
			{
				break;
			}
			else if (strcmp (printed[0], cd) == 0)
                	{
                		changeDirectory (printed[1]);
                	}
			else if (*printed[lastElement (printed)] == *background)
			{
        			/*-------Background-------*/

				fg = FALSE;

				childProcess = fork ();
				signal (SIGINT, SIG_IGN);
				if (childProcess == 0)
				{
					doCommand (printed, parent);
				} else {
				        fprintf (stderr, "**** Creating background process ****\n");
				        
					if (childProcess == -1)
					{
					        printError();
					}
				}
				waiter (fg);

				//sleep(100);
				//waitpid(-1, &status, WNOHANG);
			} else {
				/*-------Foreground-------*/ 
				
				fg = TRUE;
                		gettimeofday (&start, NULL);
				childProcess = fork ();

				signal (SIGINT, SIG_IGN);
				if (childProcess == 0)
				{
        				/* Code for child process */
        				
					doCommand (printed, parent);
				} else {
				        /* Code for parent process */
					fprintf (stderr, "**** Foreground ****\n");
                                        if (childProcess == -1)
					{
					        printError();
					}
            				fprintf (stderr, "Process executed for:\n %ld msec.\n", 1000 * ( end.tv_sec - start.tv_sec ) + ( end.tv_usec - start.tv_usec ) / 1000);
				}
				gettimeofday (&end, NULL);
				//waitpid (-1, &status, 0);
//				waiter (fg);

				/* Needed to capture Ctrl-c with signalhandler */
				waiter (fg);
			}
		}
	}
	fprintf (stderr, "Exiting simplePrompt\n");
	exit (EXIT_SUCCESS);
}
