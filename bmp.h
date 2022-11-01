#ifndef BMP
#define BMP

#include <stdint.h>
#include <stdbool.h>

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
	uint16_t channels;
	
	uint8_t* array;
	colour_t* pixels;
} BMP_t;

BMP_t* BMP_read(char* path);
void BMP_set_pixel(BMP_t* bmp, uint32_t x, uint32_t y, colour_t c);
colour_t BMP_get_pixel(BMP_t* bmp, uint32_t x, uint32_t y);
void BMP_write(BMP_t* bmp, char* path);
void BMP_dispose(BMP_t* bmp);

#endif
