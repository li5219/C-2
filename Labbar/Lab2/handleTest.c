#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void handler(int sig)
{
	pid_t pid;

	pid = wait(NULL);

	printf("Pid %d exited.\n", pid);
}

int main(void)
{
	unsigned int i=0, x=0;
	signal(SIGCHLD, handler);

	if(!fork())
	{
		for(; i < 1000000000; i++)
			{
				x += 1;
			}
		printf("Child pid is %d\n", getpid());
		exit(0);
	}
		for(i=0; i < 1000000000; i++)
			{
				x += 1;
			}
	printf("Parent pid is %d\n", getpid());

	getchar();
	return 0;
}
