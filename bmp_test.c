#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bmp.h"

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		puts("[main] Missing path argument");
		exit(EXIT_FAILURE);
	}
	
	char* input_path = argv[1];
	char* output_path = malloc(strlen(input_path) + 10);
	sprintf(output_path, "%s_test.bmp", input_path);

	BMP_t* bmp = BMP_read(input_path);

	for(int y = 0; y < bmp->height; y++)
	{
		for(int x = 0; x < bmp->width; x++)
		{
			float u = (float) x / (float) bmp->width;
			float v = (float) y / (float) bmp->height;

			colour_t c = {1, (int) (u*255), (int) (v*255), 1};
			BMP_set_pixel(bmp, x, y, c);
		}
	}

	BMP_write(bmp, output_path);
	BMP_dispose(bmp);
	
	printf
	(
		"File: %s\nSize: %d\nWidth: %d\nHeight: %d\nDepth: %d\nChannels: %d\n",
		input_path,
		bmp->file_size,
		bmp->width,
		bmp->height,
		bmp->depth,
		bmp->channels	
	);

	return 0;
}
