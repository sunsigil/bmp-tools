#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} colour_t;

typedef struct
{
	uint32_t file_size;
	uint8_t* file_content;

	uint32_t width;
	uint32_t height;
	uint16_t depth;
	
	uint8_t* array;
	colour_t* pixels;
} BMP_t;

uint16_t read_2(uint8_t* location);
uint32_t read_4(uint8_t* location);
colour_t read_bgr(uint8_t* location);
colour_t read_bgra(uint8_t* location);
void write_2(uint8_t* location, uint16_t value);
void write_4(uint8_t* location, uint32_t value);
void write_bgr(uint8_t* location, colour_t value);
void write_bgra(uint8_t* location, colour_t value);

BMP_t BMP_create(uint32_t width, uint32_t height, int has_alpha);
BMP_t BMP_read(char* path);
void BMP_print_header(BMP_t* bmp);
int BMP_set_pixel(BMP_t* bmp, uint32_t x, uint32_t y, colour_t c);
colour_t BMP_get_pixel(BMP_t* bmp, uint32_t x, uint32_t y);
int BMP_write(BMP_t* bmp, char* path);
void BMP_dispose(BMP_t* bmp);

#endif
