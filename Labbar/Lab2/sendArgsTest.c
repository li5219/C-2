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

void doCommand (char** string)
{
/*  char *sak = string[0];*/
  printf("String: %s \n", string[1]);
}

int main (int argc, char **argv)
{
  doCommand(argv);

  exit(0);
}
                                                                                                                                                        