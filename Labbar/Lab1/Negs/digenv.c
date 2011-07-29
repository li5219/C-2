/*
 * digenv är ett program som vissar enviorment variabler i
 * en sorterad ordning och om man så önskar filtrerad enligt ens önskningar.
 *
 * Dain Nilsson IT04
 * Niklas Söderlund IT04
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE_READ	0
#define PIPE_WRITE	1

/* Öppnar en pipe, om den misslyckas avslutas programet med fel kod 1 */
void OpenPipe(int *qFD)
{
	if(pipe(qFD) == 0)
		return;

	perror("Error opening pipe.\n");
	exit(1);
};

/* Stänger en pipe, om den misslyckas avslutas programet med fel kod1 */
void ClosePipe(int *qFD)
{
	if(close(qFD[PIPE_READ]) == 0)
		if(close(qFD[PIPE_WRITE]) == 0)
			return;

	perror("Error closing pipe.\n");
	exit(1);
};

/* Dupplicera en FD, om den mislyckas avsllita programet med fel kod 1 */
void Dupe(int qOld, int qNew)
{
	if(dup2(qOld, qNew) != -1)
		return;

	perror("Error duplicate a file descriptor.\n");
	exit(1);
};

/* Vänta på att ett pid skall terminera */
void WaitPid(pid_t qPid)
{
	int	Status;

	if(waitpid(qPid, &Status, 0) == -1)
	{
		perror("Waiting for pid.\n");
		exit(1);
	};

	if(WIFEXITED(Status))
		if(WEXITSTATUS(Status) != 0)
			fprintf(stderr, "Child with PID %d exit with error code: %d.\n", (int)qPid, WEXITSTATUS(Status));
};

int main(int argc, char** argv)
{
	const char*	printenv	= "printenv";
	const char*	sort		= "sort";
	const char*	grep		= "grep";
	char* 		EmpArg[] 	= {"", NULL};	
	int		usegrep		= 0;
	const char*	pager[]		= {NULL, "less", "more", NULL};
	int		npager		= 0;

	int		pipe1[2], pipe2[2], pipe3[2];
	pid_t		pid1, pid2, pid3, pid4;



	/* Fundera ut om vi skall använda grep */
	usegrep = argc > 1 ? 1 : 0;

	/* Hitta vilken pager som skall användas */
	pager[0] = (char*)getenv("PAGER");
	if(pager[0] == NULL)
		npager++;

	/* Skapa tre pipes att prata genom */
	OpenPipe(pipe1);
	OpenPipe(pipe2);
	OpenPipe(pipe3);

	/* Skapa prosess 1 printenv*/
	pid1 = fork();

	if(pid1 == 0)
	{
		/* Child */

		/* Rederict output to stdout */
		Dupe(pipe1[PIPE_WRITE], STDOUT_FILENO);

		/* Close all pipes so we dont mess up main */
		ClosePipe(pipe1);
		ClosePipe(pipe2);
		ClosePipe(pipe3);

		execvp(printenv, EmpArg);
		
		perror("Error exec printenv.");
		exit(1);
	}
	else if(pid1 == -1)
	{
		perror("Error forking");
		exit(1);
	};

	/* Skapa prosess 2 grep if we use it*/
	if(usegrep == 1)
	{
		pid2 = fork();

		if(pid2 == 0)
		{
			/* Child */
	
			/* Rederict output to stdout */
			Dupe(pipe1[PIPE_READ], STDIN_FILENO);
			Dupe(pipe2[PIPE_WRITE], STDOUT_FILENO);

			/* Close all pipes so we dont mess up main */
			ClosePipe(pipe1);
			ClosePipe(pipe2);
			ClosePipe(pipe3);

			execvp(grep, argv);

			perror("Error exec grep.");
			exit(1);
		}
		else if(pid2 == -1)
		{
			perror("Error forking");
			exit(1);
		};
	};

	/* Skapa prosess 3 sort*/
	pid3 = fork();

	if(pid3 == 0)
	{
		/* Child */

		/* Rederict output to stdout */
		Dupe(usegrep == 1 ? pipe2[PIPE_READ] : pipe1[PIPE_READ], STDIN_FILENO);
		Dupe(pipe3[PIPE_WRITE], STDOUT_FILENO);

		/* Close all pipes so we dont mess up main */
		ClosePipe(pipe1);
		ClosePipe(pipe2);
		ClosePipe(pipe3);

		execvp(sort, EmpArg); 

		perror("Error exec sort.");
		exit(1);
	}
	else if(pid3 == -1)
	{
		perror("Error forking");
		exit(1);
	};

	/* Skapa prosess 4 pager*/
	pid4 = fork();

	if(pid4 == 0)
	{
		/* Child */

		/* Rederict output to stdout */
		Dupe(pipe3[PIPE_READ], STDIN_FILENO);

		/* Close all pipes so we dont mess up main */
		ClosePipe(pipe1);
		ClosePipe(pipe2);
		ClosePipe(pipe3);

		/* try to exec a pager that exsists in a good order */
		while(pager[npager] != NULL)
		{
			execvp(pager[npager++], EmpArg); 

			if(errno != ENOENT)
				break;
		};

		perror("Error exec pager.");
		exit(1);
	}
	else if(pid1 == -1)
	{
		perror("Error forking");
		exit(1);
	};

	/* Main dont need pipes no more */
	ClosePipe(pipe1);
	ClosePipe(pipe2);
	ClosePipe(pipe3);

	/* Wait for all children to close */
	WaitPid(pid1);
	if(usegrep == 1)
		WaitPid(pid2);
	WaitPid(pid3);
	WaitPid(pid4);

	return(0);
};

