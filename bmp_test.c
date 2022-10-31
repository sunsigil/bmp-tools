#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		puts("[main] Missing path argument");
		exit(EXIT_FAILURE);
	}

	BMP_t* bmp = BMP_read(argv[1]);
	BMP_write(bmp, argv[1]);
	BMP_dispose(bmp);
	
	printf
	(
		"File: test.bmp\nSize: %d\nWidth: %d\nHeight: %d\nDepth: %d\nChannels: %d\n",
		bmp->file_size,
		bmp->width,
		bmp->height,
		bmp->depth,
		bmp->channels	
	);

	return 0;
}
