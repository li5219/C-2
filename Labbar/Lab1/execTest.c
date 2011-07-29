#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pid_t childpid;

int main()
{

  childpid = fork ();
  if (childpid == 0)
    {
      execlp ("printenv", "printenv", "", NULL);
      fprintf(stderr, "Error in 'printenv'\n");
    }
  if (-1 == childpid)
    {
      fprintf(stderr, "Cannot fork for printenv\n");
    }
  exit(0);
}
