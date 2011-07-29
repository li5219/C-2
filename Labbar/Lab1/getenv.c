#include <stdio.h>
#include <stdlib.h>

int main()
{
  char *env, *shit;
  shit = "USER";
  env=getenv(shit);
  if(env!=NULL)
  {
    printf("%s\n", env);
  } else {
    printf("Error no variable %s found!\n", shit);
  }
  
  return 0;
}
