#include <stdio.h>
#include <stdarg.h>

void printArgs (int n_args, ...)
{
  int i=0;
  va_list argList;
  va_start (argList, n_args);
  
  fprintf(stdout, "%d arguments sent to the function.\n", n_args);
  while (i < n_args)
   {
     fprintf(stdout, "Argument %d is %d\n", i, va_arg (argList, int));
     i += 1;
   }
}

int main ()
{
  printArgs(1, 2, 3, 4, 5);
  
  return 0;
}
