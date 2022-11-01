test: bmp
	@gcc-12 bmp.o bmp_test.c -o bmp_test
	-./bmp_test rgb_test.bmp
	-./bmp_test rgb_test.png
	-./bmp_test pattern.bmp

bmp:
	@gcc-12 -c bmp.c -o bmp.o
