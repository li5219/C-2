#include <stdio.h>
#include <stdarg.h>

float
average (int n_args, ...)
{
  va_list myList;
  va_start (myList, n_args);

  int numbersAdded = 0;
  int sum = 0;

  while (numbersAdded < n_args)
    {
      int number = va_arg (myList, int);	// Get next number from list
      sum += number;
      numbersAdded += 1;
    }

  float avg = (float) (sum) / (float) (numbersAdded);	// Find the average
  return avg;
}

int
main ()
{
  print (1, 2, 3, 4, 5);

  return 0;
}
