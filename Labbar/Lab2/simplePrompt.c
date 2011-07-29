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
  
/*
Function to capture Ctrl+C. This is generally disabled.
*/ 
void interrupts (int sigNum)
{
	fprintf (stderr, "\nCtrl + C is disabled\n");
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
* char *inc: Incoming string to be tested
* int i: Integer used for incrementation in the for-loop
* int len: Length of the incoming string containing the length of the string
*/
char *clearString (char *inc) 
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
Function to tokenize the command string and retur an array of the commands
* char *incoming: Incoming string to be tokenized to an array
* int empty: Variable used as a boolean to check if the incoming string is empty
* int len: Length of the command string
* int i: Used for incrementation of the loop
* char **result: The returning array of commands. Max commands is set to 5
* char *str: 
* char *temp: Temprary string
*/ 
char **parseString (char *incoming) 
{
	int empty, len = strlen (incoming) - 1, i = 0;
	char **result = calloc (sizeof (char *), 5), *str = NULL, *temp;
  	empty = checkIfEmpty (incoming);
	if (empty == FALSE)
	{
		temp = malloc (len);
		temp = clearString (incoming);
		str = strtok (temp, " ");
		while (str != NULL)
		{
			result[i] = str;
			i = i + 1;
			str = strtok (NULL, " ");
		}
	} else {
	        *result = "empty";
	}
	return result;
	free (result);
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
	char *home = malloc (strlen (getenv ("HOME")));

	home = getenv ("HOME");
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

void checkChild(pid_t cpid)
{
        int stat, cstat;

	if (WIFEXITED (stat))
	{
		cstat = WEXITSTATUS (stat);
		if (0 != cstat)
			fprintf (stderr, "Child with PID %ld failed with error: %d\n", (long) cpid, cstat);
		else
			fprintf (stderr, "Child with PID %ld terminated normaly.\n", (long) cpid);
	} else if (WIFSIGNALED (stat))
	{
		fprintf (stderr, "Child with PID %ld terminated with signal: %d\n", (long) cpid, WTERMSIG (stat));
	} else {
		fprintf (stderr, "Error while waiting for child (%d).\n", cpid);
	}
}

/*
Function to wait for child process and print the exit status
*/ 
void waitChild () 
{
	pid_t id = 1;
	int cstat;

	while (id > 0)
	{
	        id = waitpid (-1, &cstat, WNOHANG);
        	if (id > 0)
        	{
        	        checkChild(id);
        	}
	}
}

void sighandler (int num)
{
	waitChild ();
}

/*
Main function. This will run in an "infinite" loop to get commands from user.
* size_t size: Used for allocation of the absolute pathname of current directory
* int fg: Helps when calling waiter functions. Is 1 or 0 if the process is a foreground or background respectively
* char *buffer: Array containing the pathname of current directory
* char **printed: Tokenized commands from user
* char *command: The string of commands given by the user
* char *background : Strings used for simple comparing of commands. The uses are selfexplainatory
       *cd
       *empty
       *quit
* pid_t childProcess, parent: Contains processid's of parent and child
* timeval end, start: Used to get the time of the process execution
*/
int main (void)
{
	size_t size = 100;
	int fg;
	char *buffer = (char *) malloc (size), *command = malloc (allocated), **printed;
	char *background = "&", *cd = "cd", *empty = "empty",  *quit = "exit";
	pid_t childProcess, parent = getpid ();
	struct timeval end, start;
	while (TRUE)
	{
		fprintf (stderr, "%s @ %s: ", getenv ("USER"), getcwd (buffer, size));
		fgets (command, allocated, stdin);
		printed = parseString (command);
		signal (SIGINT, interrupts);
		#ifdef SH
			signal (SIGCHLD, sighandler);
		#endif
		#ifndef SH
			signal (SIGCHLD, SIG_DFL);
			waitChild ();
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
				fprintf (stderr, "**** Creating background process ****\n");
				signal (SIGINT, SIG_IGN);
				childProcess = fork ();
				if (childProcess == 0)
				{
					doCommand (printed, parent);
				} else {
					if (childProcess == -1)
					{
						printError ();
					}
				}
			} else {
				/*-------Foreground-------*/
				fg = TRUE;

				fprintf (stderr, "**** Foreground ****\n");
				gettimeofday (&start, NULL);
				childProcess = fork ();
				signal (SIGINT, SIG_IGN);

				if (childProcess == 0)
				{
					/* Code for child process */
					doCommand (printed, parent);
				} else {
					/* Code for parent process */
					if (childProcess == -1)
					{
						printError ();
					}
					waitChild ();
					gettimeofday (&end, NULL);
					fprintf (stderr, "Process executed for:\n %ld msec.\n", 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000);
				}
			}
		}
        }
    fprintf (stderr, "Exiting simplePrompt\n");
    exit (EXIT_SUCCESS);
}
