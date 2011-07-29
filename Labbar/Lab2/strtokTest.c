#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char str[] = "Now # is the time for all # good men to come to the # aid of their country";
	char delims[] = " ";
	char *result = NULL;
	result = strtok( str, delims );
	while( result != NULL )
	{
		printf("Result is \"%s\"\n", result);
		result = strtok( NULL, delims );
	}
	exit(EXIT_SUCCESS);
}
