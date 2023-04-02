#include "bmp_mapped.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

BMP_mapped_t BMPM_map(char* path)
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
	bmp.channels = depth/8;
	bmp.row_padding = (width % 4 == 0) ? 0 : 4 - (width % 4);
	bmp.array = bytes + array_offset;
	bmp.pixels = NULL;
	BMP_mapped_t bmpm = {};
	bmpm.path = path;
	bmpm.fd = fd;
	bmpm.bmp = bmp;

	return bmpm;
}

colour_t BMPM_get_pixel(BMP_mapped_t* bmpm, int x, int y)
{
	int width = bmpm->bmp.width;
	int channels = bmpm->bmp.channels;
	int row_padding = bmpm->bmp.row_padding;
	
	int row_index = y * (width * channels + row_padding);
	int array_index = row_index + x * channels;

	if(channels == 3)
	{ return read_bgr(bmpm->bmp.array + array_index); }
	return read_bgra(bmpm->bmp.array + array_index);
}

int BMPM_unmap(BMP_mapped_t* bmpm)
{
	if(munmap(bmpm->bmp.file_content, bmpm->bmp.file_size) == -1)
	{
		perror("[BMPM_unmap] munmap");
		exit(EXIT_FAILURE);
	}
	if(close(bmpm->fd) == -1)
	{
		perror("[BMPM_unmap] close");
		exit(EXIT_FAILURE);
	}
	return 0;
}
