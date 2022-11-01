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
			colour_t original = BMP_get_pixel(bmp, x, y);
			colour_t inverse = {255-original.r, 255-original.g, 255-original.b, original.a};
			BMP_set_pixel(bmp, x, y, inverse);
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
