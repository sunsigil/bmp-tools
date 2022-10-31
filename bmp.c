#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

#define ARRAY_OFFSET_OFFSET 10
#define ARRAY_OFFSET_SIZE 4

#define WIDTH_OFFSET 18
#define WIDTH_SIZE 4

#define HEIGHT_OFFSET 22
#define HEIGHT_SIZE 4

#define DEPTH_OFFSET 28
#define DEPTH_SIZE 2

BMP_t* BMP_load(char* path)
{
	FILE* file = fopen(path, "rb");
	
	if(file == NULL)
	{
		perror("[BMP_load] Failed to open file");
		exit(EXIT_FAILURE);
	}

	BMP_t* bmp = malloc(sizeof(BMP_t));
	
	fseek(file, 0, SEEK_END);
	bmp->file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	bmp->file_content = malloc(bmp->file_size);
	fread(bmp->file_content, 1, bmp->file_size, file);
	
	fclose(file);

	if(!(bmp->file_content[0] == 'B' && bmp->file_content[1] == 'M'))
	{
		puts("[BMP_load] File does not have BM signature");
		exit(EXIT_FAILURE);
	}	

	uint8_t* width_location = bmp->file_content + WIDTH_OFFSET;
	bmp->width = (uint32_t) *width_location;
	uint8_t* height_location = bmp->file_content + HEIGHT_OFFSET;
	bmp->height = (uint32_t) *height_location;
	uint8_t* depth_location = bmp->file_content + DEPTH_OFFSET;
	bmp->depth = (uint16_t) *depth_location;
	bmp->channels = bmp->depth / 8;
	
	if(!(bmp->channels == 3 ||bmp->channels == 4))
	{
		puts("[BMP_load] Colours are neither RGB nor RGBA");
		exit(EXIT_FAILURE);
	}
	
	bmp->pixels = malloc(sizeof(color_t) * bmp->width * bmp->height);
	uint32_t array_offset = (uint32_t) *(bmp->file_content + ARRAY_OFFSET_OFFSET);
	uint8_t* array = bmp->file_content + array_offset;

	for(int x = 0; x < bmp->width; x++)
	{
		for(int y = 0; y < bmp->height; y++)
		{
			int index = x * bmp->width + y;
			uint8_t* pixel_start = array + (index * bmp->channels);

			bmp->pixels[index].r = *(pixel_start+0);
			bmp->pixels[index].g = *(pixel_start+1);
			bmp->pixels[index].b = *(pixel_start+2);
			bmp->pixels[index].a = (bmp->channels == 4) ? *(pixel_start+3) : 1;
		}
	}

	return bmp;
}

void BMP_write(BMP_t* bmp, char* path)
{

	FILE* file = fopen(path, "wb");
	
	if(file == NULL)
	{
		perror("[BMP_write] Failed to open file");
		exit(EXIT_FAILURE);
	}

	fwrite(bmp->file_content, 1, bmp->file_size, file);
	fclose(file);	
}

void BMP_dispose(BMP_t* bmp)
{
	free(bmp->file_content);
	free(bmp->pixels);
	free(bmp);	
}

