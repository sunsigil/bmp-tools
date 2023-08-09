#include "bmp_mapped.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

BMP_t BMPM_map(char* path)
{
	int fd = open(path, O_RDONLY, S_IRUSR);
	if(fd == -1)
	{
		perror("[BMP_map] open");
		exit(EXIT_FAILURE);
	}
	
	uint32_t byte_count = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	uint8_t* bytes = mmap(NULL, byte_count, PROT_READ, MAP_PRIVATE, fd, SEEK_SET);
	if(bytes == NULL)
	{
		perror("[BMP_map] mmap");
		exit(EXIT_FAILURE);
	}
	if(close(fd) == -1)
	{
		perror("[BMPM_map] close");
		exit(EXIT_FAILURE);
	}

	uint16_t signature = read_2(bytes+SIGNATURE_OFFSET);
	uint32_t file_size = read_4(bytes+FILE_SIZE_OFFSET);
	uint32_t array_offset = read_4(bytes+ARRAY_OFFSET_OFFSET);
	uint32_t width = read_4(bytes+WIDTH_OFFSET);
	uint32_t height = read_4(bytes+HEIGHT_OFFSET);
	uint16_t depth = read_2(bytes+DEPTH_OFFSET);

	if(signature != 0x4D42)
	{
		puts("[BMP_map] File does not have correct signature");
		free(bytes);
		exit(EXIT_FAILURE);
	}	
	if(!(depth == 24 || depth == 32))
	{
		puts("[BMP_map] Colours are neither RGB nor RGBA");
		free(bytes);
		exit(EXIT_FAILURE);
	}

	BMP_t bmp = {};
	bmp.file_size = file_size;
	bmp.file_content = bytes;
	bmp.width = width;
	bmp.height = height;
	bmp.depth = depth;
	bmp.array = bytes + array_offset;
	bmp.pixels = NULL;

	return bmp;
}

colour_t BMPM_get_pixel(BMP_t* bmp, int x, int y)
{
	if(x < 0 || x >= bmp->width || y < 0 || y >= bmp->height)
	{
		puts("[BMPM_get_pixel] Attempted to read out-of-bounds");
		return (colour_t) {0, 0, 0, 0};
	}

	size_t pixel_size = bmp->depth/8;
	size_t row_size = ((bmp->depth * bmp->width + 31) / 32) * 4;

	int array_index = y * row_size + x * pixel_size;

	if(pixel_size == 3)
	{ return read_bgr(bmp->array+array_index); }
	return read_bgra(bmp->array+array_index);
}

int BMPM_unmap(BMP_t* bmp)
{
	if(munmap(bmp->file_content, bmp->file_size) == -1)
	{
		perror("[BMPM_unmap] munmap");
		exit(EXIT_FAILURE);
	}

	return 0;
}
