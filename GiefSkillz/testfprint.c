#include <stdio.h>
#include <math.h>

int main()
{
  float x = 3243248190.82309;
  float x2;
  int printValue, printValue2;
  
  x2 = fabsf(x);

  if ((printValue = fprintf(stdout, "x: %f\n", x)) != 0)
    fprintf(stdout, "Printed %d\n", printValue);
  
  if ((printValue2 = fprintf(stdout, "x2: %f\n", x2)) != 0)
    fprintf(stdout, "Printed %d\n", printValue2);

  return 0;
}
