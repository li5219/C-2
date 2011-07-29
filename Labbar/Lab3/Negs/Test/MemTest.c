
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define BLOCKS 10000

#ifndef MAXBLOCKSIZE
#error "MAXBLOCKSZ not def."
#endif

int main()
{
	int i, n;
	unsigned long TotalSize = 0;
	size_t BlockSize[BLOCKS] = {0};
	void *Start, *Stop, *Blocks[BLOCKS];

	srand((unsigned int) time(NULL));

	Start = sbrk( 0 );
	for(n = 0; n < 10; n++)
		for(i = 0; i < BLOCKS; i++)
		{
			if(n == 0)
				Blocks[i] = NULL;

			BlockSize[i] = (rand() % MAXBLOCKSIZE) + 1;

			Blocks[i] = realloc(Blocks[i], BlockSize[i]);
		};
	Stop = sbrk( 0 );

	for(i = 0; i < BLOCKS; i++)
	{
		TotalSize += BlockSize[i];
		free(Blocks[i]);
	};

	printf("Allocated memory: %ld bytes\n", TotalSize);
	printf("Consumed memory: %ld bytes\n", (unsigned long)(Stop - Start));
	printf("Wasted memory: %ld bytes\n", (unsigned long)(Stop - Start) - TotalSize);

	return(0);
}
