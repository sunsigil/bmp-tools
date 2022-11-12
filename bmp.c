// This implementation assumes BITMAPFILEHEADER followed by BITMAPINFOHEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bmp.h"

#define HEADER_OFFSET 0
#define HEADER_SIZE 14
#define INFO_HEADER_OFFSET 14
#define INFO_HEADER_SIZE 40

#define SIGN_OFFSET 0
#define SIGN_SIZE 2

#define FILE_SIZE_OFFSET 2
#define FILE_SIZE_SIZE 4

#define RESERVED_OFFSET 6
#define RESERVED_SIZE 4

#define ARRAY_OFFSET_OFFSET 10
#define ARRAY_OFFSET_SIZE 4

#define INFO_HEADER_SIZE_OFFSET 14
#define INFO_HEADER_SIZE_SIZE 4

#define WIDTH_OFFSET 18
#define WIDTH_SIZE 4

#define HEIGHT_OFFSET 22
#define HEIGHT_SIZE 4

#define PLANES_OFFSET 26
#define PLANES_SIZE 2

#define DEPTH_OFFSET 28
#define DEPTH_SIZE 2

#define COMPRESSION_OFFSET 30
#define COMPRESSION_SIZE 4

#define COMPRESSED_SIZE_OFFSET 34
#define COMPRESSED_SIZE_SIZE 4

#define X_PPM_OFFSET 38
#define X_PPM_SIZE 4

#define Y_PPM_OFFSET 42
#define Y_PPM_SIZE 4

#define COLOURS_OFFSET 46
#define COLOURS_SIZE 4

#define SIGNIFICANT_COLOURS_OFFSET 50
#define SIGNIFICANT_COLOURS_SIZE 4

uint16_t read_2(uint8_t* location)
{
	uint16_t value = location[0] | (location[1] << 8);
	return value;
}

uint32_t read_4(uint8_t* location)
{
	uint32_t value = location[0] | (location[1] << 8) | (location[2] << 16) | (location[3] << 24);
	return value;
}

colour_t read_bgr(uint8_t* location)
{
	colour_t value = { location[2], location[1], location[0], 1 };
	return value;
}

colour_t read_bgra(uint8_t* location)
{
	colour_t value = { location[2], location[1], location[0], location[3] };
	return value;
}

void write_2(uint8_t* location, uint16_t value)
{
	location[0] = (value & 0x00FF);
	location[1] = (value & 0xFF00) >> 8;
}

void write_4(uint8_t* location, uint32_t value)
{
	location[0] = (value & 0x000000FF);
	location[1] = (value & 0x0000FF00) >> 8;
	location[2] = (value & 0x00FF0000) >> 16;
	location[3] = (value & 0xFF000000) >> 24;
}

void write_bgr(uint8_t* location, colour_t value)
{
	location[0] = value.b;
	location[1] = value.g;
	location[2] = value.r;
}

void write_bgra(uint8_t* location, colour_t value)
{
	location[0] = value.b;
	location[1] = value.g;
	location[2] = value.r;
	location[3] = value.a;
}

BMP_t* BMP_create(uint32_t width, uint32_t height, uint16_t channels)
{
	uint16_t sign = 0x4D42;
	// uint32_t reserved = 0;
	uint32_t array_offset = INFO_HEADER_OFFSET + INFO_HEADER_SIZE;

	int row_padding = (width % 4 == 0) ? 0 : (4 - width % 4);
	int row_size = channels * width + row_padding;
	uint32_t array_size = row_size * height;
	
	uint32_t file_size = HEADER_SIZE + INFO_HEADER_SIZE + array_size;
	uint32_t info_header_size = INFO_HEADER_SIZE;
	
	uint16_t planes = 1;
	uint16_t depth = 8 * channels;
	// uint32_t compression = 0;
	// uint32_t compressed_size = 0;
	// uint32_t x_ppm = width;
	// uint32_t y_ppm = height;
	// uint32_t colours = (channels == 4) ? 0xFFFFFFFF : 0xFFFFFF ;
	// uint_t significant_colours = 0;	

	BMP_t* bmp = malloc(sizeof(BMP_t));	
	bmp->file_size = file_size;
	bmp->file_content = calloc(file_size, 1);
	bmp->width = width;
	bmp->height = height;
	bmp->depth = depth;
	bmp->channels = channels;
	bmp->array = bmp->file_content + array_offset;
	bmp->pixels = calloc(width * height, sizeof(colour_t));
	
	write_2(bmp->file_content+SIGN_OFFSET, sign);
	write_4(bmp->file_content+FILE_SIZE_OFFSET, file_size);
	// write_4(bmp->file_content+RESERVED_OFFSET, reserved);
	write_4(bmp->file_content+ARRAY_OFFSET_OFFSET, array_offset);
	write_4(bmp->file_content+INFO_HEADER_SIZE_OFFSET, info_header_size);
	write_4(bmp->file_content+WIDTH_OFFSET, width);
	write_4(bmp->file_content+HEIGHT_OFFSET, height);
	write_2(bmp->file_content+PLANES_OFFSET, planes);
	write_2(bmp->file_content+DEPTH_OFFSET, depth);
	// write_4(bmp->file_content+COMPRESSION_OFFSET, compression);
	// write_4(bmp->file_content+COMPRESSED_SIZE_OFFSET, compressed_size);
	// write_4(bmp->file_content+X_PPM_OFFSET, x_ppm);
	// write_4(bmp->file_content+Y_PPM_OFFSET, y_ppm);
	// write_4(bmp->file_content+COLOURS_OFFSET, colours);
	// write_4(bmp->file_content+SIGNIFICANT_COLOURS_OFFSET, significant_colours);	

	return bmp;
}

BMP_t* BMP_read(char* path)
{
	FILE* file = fopen(path, "rb");
	
	if(file == NULL)
	{
		perror("[BMP_read] Failed to open file");
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
		puts("[BMP_read] File does not have BM signature");
		exit(EXIT_FAILURE);
	}	

	uint8_t* width_location = bmp->file_content + WIDTH_OFFSET;
	bmp->width = (uint32_t) *width_location;
	uint8_t* height_location = bmp->file_content + HEIGHT_OFFSET;
	bmp->height = (uint32_t) *height_location;
	uint8_t* depth_location = bmp->file_content + DEPTH_OFFSET;
	bmp->depth = (uint16_t) *depth_location;
	bmp->channels = bmp->depth / 8;
	
	if(!(bmp->channels == 3 || bmp->channels == 4))
	{
		puts("[BMP_read] Colours are neither RGB nor RGBA");
		exit(EXIT_FAILURE);
	}
	
	bmp->pixels = malloc(sizeof(colour_t) * bmp->width * bmp->height);
	uint32_t array_offset = (uint32_t) *(bmp->file_content + ARRAY_OFFSET_OFFSET);
	bmp->array = bmp->file_content + array_offset;

	for(int y = 0; y < bmp->height; y++)
	{
		for(int x = 0; x < bmp->width; x++)
		{
			int index = y * bmp->width + x;
			uint8_t* array_position = bmp->array + (index * bmp->channels);

			bmp->pixels[index].r = *(array_position+0);
			bmp->pixels[index].g = *(array_position+1);
			bmp->pixels[index].b = *(array_position+2);
			if(bmp->channels == 4)
			{ bmp->pixels[index].a = *(array_position + 3); }
		}
	}

	return bmp;
}

void BMP_inspect_header(BMP_t* bmp)
{
	printf
	(
		"[HEADER]\nSIGN: %d\nSIZE: %d\nRESERVED: %d\nARRAY OFFSET: %d\nINFO HEADER SIZE %d\nWIDTH: %d\nHEIGHT: %d\nPLANES %d\nDEPTH: %d\nCOMPRESSION: %d\nCOMPRESSED SIZE: %d\nX PPM: %d\nY PPM: %d\nCOLOURS: %d\nSIGNIFICANT COLOURS: %d\n",
		read_2(bmp->file_content+SIGN_OFFSET),
		read_4(bmp->file_content+FILE_SIZE_OFFSET),
		read_4(bmp->file_content+RESERVED_OFFSET),
		read_4(bmp->file_content+ARRAY_OFFSET_OFFSET),
		read_4(bmp->file_content+INFO_HEADER_SIZE_OFFSET),
		read_4(bmp->file_content+WIDTH_OFFSET),
		read_4(bmp->file_content+HEIGHT_OFFSET),
		read_2(bmp->file_content+PLANES_OFFSET),
		read_2(bmp->file_content+DEPTH_OFFSET),
		read_4(bmp->file_content+COMPRESSION_OFFSET),
		read_4(bmp->file_content+COMPRESSED_SIZE_OFFSET),
		read_4(bmp->file_content+X_PPM_OFFSET),
		read_4(bmp->file_content+Y_PPM_OFFSET),
		read_4(bmp->file_content+COLOURS_OFFSET),
		read_4(bmp->file_content+SIGNIFICANT_COLOURS_OFFSET)
	);
}

void BMP_set_pixel(BMP_t* bmp, uint32_t x, uint32_t y, colour_t c)
{
	if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
	{
		puts("[BMP_set_pixel] Attempted to write out-of-bounds");
		exit(EXIT_FAILURE);
	}

	int index = y * bmp->width + x;
	colour_t* pixels_position = bmp->pixels + index;
	uint8_t* array_position = bmp->array + (index * bmp->channels);
	
	*pixels_position = c;

	*(array_position+0) = c.r;
	*(array_position+1) = c.g;
	*(array_position+2) = c.b;
	if(bmp->channels == 4)
	{ *(array_position+3) = c.a; }
}

colour_t BMP_get_pixel(BMP_t* bmp, uint32_t x, uint32_t y)
{
	if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
	{
		puts("[BMP_set_pixel] Attempted to write out-of-bounds");
		exit(EXIT_FAILURE);
	}

	int index = y * bmp->width + x;
	colour_t* pixels_position = bmp->pixels + index;
	
	return *pixels_position;	
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

