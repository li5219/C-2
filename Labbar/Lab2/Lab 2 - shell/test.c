#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char **argv){
	
	int childpid, wait_pid;
	int status;
	int counter = 0;
	char *text;
	text = malloc(1024);
	time_t t1,t2;
	int time = 0;
	while(1){
		fgets(text, 10, stdin);
		int i;	
		for(i = 0; i < strlen(text); i++){
			if (text[i] == '\n')
				text[i] = '\0';
		}
		if(strcmp(text, "exit") == 0){
			exit(0);
		}
		
		childpid = fork();
		if(childpid == 0){
			(void) time(&t1);
			execvp(text, argv);
			fprintf(stderr, "Not a valid command, please try again\n\n");
		}
		
		else if (childpid < (pid_t) 0){
			/* The fork failed. */ 
			fprintf (stderr, "Fork for environent printing failed.\n");
			return EXIT_FAILURE;
		}
		time = (int)t2.time_utime - (int)t1.time_utime;
		printf("tard: %d",time);
		wait_pid = wait(&status);
				printf("Pid: %d",wait_pid);
		
		while(counter < 1){			
			if(wait_pid == -1){
				perror("Waiterror");
				exit(1);
			}
			counter++;
		}
	}
	free(text);
	return 0;
	
	//skriv ut pid på procsser, när deen skapades och nrä den dödades
	//hur lång tid det tog att köra en förgrundsprocess.
	
	
}
