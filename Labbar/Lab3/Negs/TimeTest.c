
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define BLOCKS 10000

#ifndef MAXBLOCKSIZE
#error "MAXBLOCKSIZE not def."
#endif

int main()
{
	int i;
	void *Blocks[BLOCKS];

	srand((unsigned int) time(NULL));

	for( i = 0; i < BLOCKS; i++ )
		Blocks[i] = malloc((rand() % MAXBLOCKSIZE) + 1);

	for(i = 0; i < BLOCKS; i++)
		if(rand() % 2 == 0)
		{
			free(Blocks[i]);
			Blocks[i] = NULL;
		}

	for(i = 0; i < BLOCKS; i++)
		if(Blocks[i] != NULL)
			Blocks[i] = realloc(Blocks[i], (rand() % MAXBLOCKSIZE) + 1);

	for(i = 0; i < BLOCKS; i++)
		if( Blocks[i] != NULL)
			free(Blocks[i]);

	return(0);
}
