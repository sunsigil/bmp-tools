// This implementation assumes BITMAPFILEHEADER followed by BITMAPINFOHEADER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bmp.h"

#define HEADER_OFFSET 0
#define HEADER_SIZE 14
#define INFO_HEADER_OFFSET 14
#define INFO_HEADER_SIZE 40

#define SIGNATURE_OFFSET 0
#define SIGNATURE_SIZE 2

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
	uint16_t signature = 0x4D42;
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
	
	write_2(bmp->file_content+SIGNATURE_OFFSET, signature);
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
	
	fseek(file, 0, SEEK_END);
	uint32_t byte_count = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8_t* bytes = malloc(byte_count);
	fread(bytes, 1, byte_count, file);
	fclose(file);
	
	uint16_t signature = read_2(bytes+SIGNATURE_OFFSET);
	uint32_t file_size = read_4(bytes+FILE_SIZE_OFFSET);
	uint32_t array_offset = read_4(bytes+ARRAY_OFFSET_OFFSET);
	uint32_t width = read_4(bytes+WIDTH_OFFSET);
	uint32_t height = read_4(bytes+HEIGHT_OFFSET);
	uint16_t depth = read_2(bytes+DEPTH_OFFSET);

	if(signature != 0x4D42)
	{
		puts("[BMP_read] File does not have correct signature");
		exit(EXIT_FAILURE);
	}
	
	if(!(depth == 24 || depth == 32))
	{
		puts("[BMP_read] Colours are neither RGB nor RGBA");
		exit(EXIT_FAILURE);
	}

	int channels = depth/8;
	int row_remainder = width % 4;
	int row_padding = row_remainder == 0 ? 0 : 4 - row_remainder;

	uint8_t* array = bytes+array_offset;
	colour_t* pixels = malloc(sizeof(colour_t) * width * height);

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			int pixel_index = y * width + x;
			int row_index = y * (width * channels + row_padding);
			int array_index = row_index + x * channels;

			if(channels == 3)
			{ pixels[pixel_index] = read_bgr(array+array_index); }
			else
			{ pixels[pixel_index] = read_bgra(array+array_index); }
		}
	}

	BMP_t* bmp = malloc(sizeof(BMP_t));
	bmp->file_size = file_size;
	bmp->file_content = bytes;
	bmp->width = width;
	bmp->height = height;
	bmp->depth = depth;
	bmp->channels = channels;
	bmp->row_padding = row_padding;
	bmp->array = array;
	bmp->pixels = pixels;

	return bmp;
}

void BMP_print_header(BMP_t* bmp)
{
	printf
	(
		"SIGNATURE: %d\nFILE SIZE: %d\nRESERVED BYTES: %d\nARRAY OFFSET: %d\nINFO HEADER SIZE: %d\nWIDTH: %d\nHEIGHT: %d\nPLANES: %d\nCOLOUR DEPTH: %d\nCOMPRESSION: %d\nCOMPRESSED SIZE: %d\nX PPM: %d\nY PPM: %d\nCOLOURS: %d\nSIGNIFICANT COLOURS: %d\n",
		read_2(bmp->file_content+SIGNATURE_OFFSET),
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

	int pixel_index = y * bmp->width + x;
	int row_size = bmp->width * bmp->channels + bmp->row_padding;
	int row_index = y * row_size;
	int array_index = row_index + x * bmp->channels;
	
	bmp->pixels[pixel_index] = c;
	if(bmp->channels == 3)
	{ write_bgr(bmp->array+array_index, c); }
	else
	{ write_bgra(bmp->array+array_index, c); }
}

colour_t BMP_get_pixel(BMP_t* bmp, uint32_t x, uint32_t y)
{
	if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
	{
		puts("[BMP_set_pixel] Attempted to write out-of-bounds");
		exit(EXIT_FAILURE);
	}

	int pixel_index = y * bmp->width + x;	
	return *(bmp->pixels+pixel_index);
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

