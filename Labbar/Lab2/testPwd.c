#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
  long size;
  char *buf, *ptr;

  ptr = getcwd();
    
  printf("%s\n", (char *) ptr[1]);
  exit(0);
}
