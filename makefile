test: bmp
	@gcc bmp.o bmp_test.c -o bmp_test
	-./bmp_test rgb_test.bmp
	-./bmp_test rgb_test.png
	-./bmp_test pattern.bmp

bmp:
	@gcc -c bmp.c -o bmp.o
