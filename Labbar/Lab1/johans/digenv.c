#include <sys/types.h> /* For defining of type pid_t */
#include <sys/wait.h> /* For defining WIFEXITED */
#include <errno.h> /* Used for perror() */
#include <stdio.h> /* For defining of stderr */
#include <stdlib.h> /* definierar bland annaoch RAND_MAX */
#include <unistd.h> /* Used for defining of pipe() */

#define PIPE_READ_SIDE ( 0 )
#define PIPE_WRITE_SIDE ( 1 )

pid_t childpid; /* Child-process PID when started with fork() */

/*------PAGER START------*/
/* This function is used to check which pager we should use.
 * If there are no pager defined in environmental variables,
 * we will use less. */

char* choosePager(){
	char *pager, *test;
	test = getenv("PAGER");
	if(test == NULL)
		pager = "less";
	else
		pager = test;
	return pager;
}
/*------PAGER END-------*/

/*-------------pipeCloser START--------------*/
/* Function for closing the READ and WRITE side of the specified pipe */
void pipeCloser(int p[]){
	int return_value;
	return_value = close( p[ PIPE_READ_SIDE ] );
	if( -1 == return_value) {
		perror( "Cannot close READ side pipe " ); exit( 1 );
		}
	return_value = close( p[ PIPE_WRITE_SIDE ] );
	if( -1 == return_value) {
		perror( "Cannot close WRITE side of pipe " ); exit( 1 );
		}
}
/*-------------pipeCloser END--------------*/	


int main(int argc, char **parameterlista)
{
	/* Variables for the program. Very usefull. */
	int return_value; /* Used to store the returnvalues from systemcalls. */
	int env_grep[2]; /* Our filedescriptors for the pipes. */
	int grep_sort[2];
	int sort_less[2];
	int status;
	char *pager = choosePager();
	
	
	/*-------------PIPE CREATION START--------------*/	
	/* We create all pipes that we will use for the program */
	return_value = pipe( env_grep );
	if( -1 == return_value ) { 
		perror( "Cannot create pipe env_grep" ); exit( 1 ); 
	}

	return_value = pipe( grep_sort);
	if( -1 == return_value) { 
		perror( "Cannot create pipe grep_sort" ); exit( 1 ); 
	}

	return_value = pipe( sort_less);
	if( -1 == return_value) { 
		perror( "Cannot create pipe sort_less" ); exit( 1 ); 
	}
	/*-------------PIPE CREATION END--------------*/
	
	/*-------------ENV PROCESS START--------------*/
	/* Write the environmental varibales into our env_grep pipe */
	childpid = fork();
	if( 0 == childpid )
	{
		/* Duplicate the filedescriptor so that pipe is written to
		 * instead of stdout */
		return_value = dup2( env_grep[ PIPE_WRITE_SIDE ],
					STDOUT_FILENO ); /* STDOUT_FILENO == 1 */
		if( -1 == return_value ) { 
			perror( "Cannot dup filedescriptor for printenv" ); exit( 1 ); 
		}

  		pipeCloser(env_grep);

  		execlp("printenv", "printenv", NULL);
  		fprintf(stderr, "execlp of printenv failed "); exit( 1 ) ;
	}
	else if (childpid < 0) {
    	perror( "Cannot create env" ); exit( 1 ); 
	}
 	/*---------------ENV PROCESS END--------------*/ 
	
	/*-------------GREP PROCESS STARTS--------------*/
	/* Redirect the STDIN to env_grep-pipe and STDOUT to grep_sort-pipe.
	 * Takes the input values in this case the enviromental variables
	 * and runs system call grep */
	childpid = fork();

	if( 0 == childpid )
	{
		return_value = dup2( grep_sort[ PIPE_WRITE_SIDE ],
					STDOUT_FILENO );	
		if( -1 == return_value ) {
			perror( "Cannot dup filedescriptor for grep" ); exit( 1 ); 
		}
		
		return_value = dup2( env_grep[ PIPE_READ_SIDE ],
					STDIN_FILENO );
		if( -1 == return_value ) {
			perror( "Cannot dup filedescriptor for grep" ); exit( 1 ); 
		}
	
    	pipeCloser(grep_sort);
    	pipeCloser(env_grep);
  
		if( argc >= 2 ) {
			parameterlista[0] = "grep";
			execvp("grep", parameterlista);
			fprintf(stderr, "execvp of grep failed "); exit( 1 ) ;
		}
		else {
			execlp("grep", "grep", "", NULL);
			fprintf(stderr, "execlp of grep failed "); exit( 1 ) ;
		}
 	}
 	else if (childpid < 0) {
    	perror( "Cannot create grep" ); exit( 1 ); 
	}
  	/*---------------GREP PROCESS END--------------*/ 
  	
	/*-------------SORT PROCESS STARTS--------------*/
	/* Redirect the STDIN to grep_sort and STDOUT to sort_less, takes the input values
	 * in this case the enviromental variables left after grep and does system call sort*/
	childpid = fork();
	
	if( 0 == childpid )
	{
		return_value = dup2( sort_less[ PIPE_WRITE_SIDE ],
					STDOUT_FILENO );	
		if( -1 == return_value ) {
			perror( "Cannot dup filedescriptor for sort" ); exit( 1 ); 
		}
		
		return_value = dup2( grep_sort[ PIPE_READ_SIDE ],
					STDIN_FILENO ); /* STDOIN_FILENO == 0 */
		if( -1 == return_value ) {
			perror( "Cannot dup" ); exit( 1 ); 
		}
	
		pipeCloser(grep_sort);
    	pipeCloser(env_grep);
    	pipeCloser(sort_less);
    	
		execlp("sort", "sort", NULL);
		fprintf(stderr, "execlp of sort failed "); exit( 1 );
  	}
  	else if (childpid < 0) {
    	perror( "Cannot create sort" ); exit( 1 ); 
	}
  	/*-------------SORT PROCESS END--------------*/
  
  	/*-------------LESS PROCESS STARTS--------------*/
  	/* Redirect the STDIN to sort_less, takes the input values
	 * in this case the enviromental variables left after grep and put in the
	 * correct order after sort. Doing system call less if less exits if not,
	 * use more.*/
  	childpid = fork();
  	
	if( 0 == childpid ) {
		
		return_value = dup2( sort_less[ PIPE_READ_SIDE ],
		STDIN_FILENO );
		if( -1 == return_value ) {
			perror( "Cannot dup" ); exit( 1 ); 
		}
	
		pipeCloser(grep_sort);
    	pipeCloser(env_grep);
    	pipeCloser(sort_less);
    		
		execlp(pager, pager, NULL);
		if(ENOENT == errno)
		{
			fprintf(stderr, "Pager not found, using 'less'");
			execlp("less", "less", NULL);
			if(ENOENT == errno)
			{
				fprintf(stderr, "Pager not found, using 'less'");
				execlp("more", "more", NULL);
			}
		}
		fprintf(stderr, "execlp of less failed "); exit( 1 );
 	}
 	else if (childpid < 0) {
    	perror( "Cannot create less" ); exit( 1 ); 
	}
  	/*-------------LESS PROCESS END--------------*/
 
  	/*-----------CLOSE UNUSED PIPES------------*/
  	/* Closing all pipes in parrent to make sure we don't wait for
  	 * any pipes to close. */
	pipeCloser(grep_sort);
    	pipeCloser(env_grep);
    	pipeCloser(sort_less);
   	/*---------CLOSE UNUSED PIPES END-----------*/
   	
 	/*---------WAIT START-----------*/
 	/* We need to wait for our child processes. */
   	int i;
	for(i = 0; i <= 3; i++) {
		childpid = wait( &status );
   		if( -1 == childpid ) {
   			perror( "wait() failed unexpectedly" ); exit( 1 );
		}
		if( WIFEXITED( status ) ) {
			int child_status = WEXITSTATUS( status );
			if( 0 != child_status ) {
				fprintf( stderr, "Child (pid %ld) failed with exit code %d\n",
				(long int) childpid, child_status );
			}
		}
		else {
			if( WIFSIGNALED( status ) ) {
				int child_status = WTERMSIG( status );
				if( 0 != child_status ) {
				fprintf( stderr, "Child (pid %ld) was terminated by signal no. %d\n",
				(long int) childpid, child_status );
				}
			}
		}
	}
	/*---------WAIT END-----------*/
  	exit( 0 );
}
