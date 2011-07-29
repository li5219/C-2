#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  char *nisse[] = {"", NULL};
  execvp("ls", nisse);
  
  return 0;
}
