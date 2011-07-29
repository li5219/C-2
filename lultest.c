#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main ()
{
  int p = sysconf (_SC_PAGESIZE);
  int q = 1;

  for (; q <= 8; q++)
    {
      printf ("P >> %d =%d\n", q, p >> q);
    }
  exit (0);
}
