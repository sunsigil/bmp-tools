#ifndef BMP_MAPPED_H
#define BMP_MAPPED_H

#include "bmp.h"

BMP_t BMPM_map(char* path);
colour_t BMPM_get_pixel(BMP_t* bmp, int x, int y);
int BMPM_unmap(BMP_t* bmp);

#endif
