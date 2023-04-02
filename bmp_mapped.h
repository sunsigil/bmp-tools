#ifndef BMP_MAPPED_H
#define BMP_MAPPED_H

#include "bmp.h"

typedef struct BMP_mapped
{
	char* path;
	int fd;
	BMP_t bmp;

} BMP_mapped_t;

BMP_mapped_t BMPM_map(char* path);
colour_t BMPM_get_pixel(BMP_mapped_t* bmpm, int x, int y);
int BMPM_unmap(BMP_mapped_t* bmpm);

#endif
