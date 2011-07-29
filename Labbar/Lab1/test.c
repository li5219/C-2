#include <stdio.h>
#include <unistd.h>

void nisse(char *hej)
{
	fprintf(stderr, hej);
}

int main (int argc, char *argv[], char *envp[])
{
/*  int a;

  printf ("The command name (argv[0]) is %s\n", argv[0]);
  printf ("There are %d arguments:\n", argc - 1);
  for (a = 1; a < argc; a++)
    printf ("\targument %2d:\t%s\n", a, argv[a]);

  printf ("The environment is as follows:\n");
  a = 0;
  while (envp[a] != NULL)
    printf ("\t%s\n", envp[a++]);
  */
  char *j="bak\n";
  nisse(j);
  return 0;
}
