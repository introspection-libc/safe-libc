#include <stdio.h>
#include <stdlib.h>

extern unsigned long _size_right(const void* addr);

int main(int argc, char** argv)
{
	int x;
	int y;
	int z[3];
	char* q = (char*) malloc(3);
	char* r = (char*) malloc(19);
	char* p = (char*) &x;
	printf("sizeof(x) = %lu\n", _size_right(&x));
	printf("sizeof(y) = %lu\n", _size_right(&y));
	printf("sizeof(z) = %lu\n", _size_right(&z));
	printf("sizeof(q) = %lu\n", _size_right(q));
	printf("sizeof(r) = %lu\n", _size_right(r));
	printf("sizeof(p[1]) = %lu\n", _size_right(&p[1]));
	printf("sizeof(p[2]) = %lu\n", _size_right(&p[2]));
	printf("sizeof(p[3]) = %lu\n", _size_right(&p[3]));
	printf("sizeof(p[4]) = %lu\n", _size_right(&p[4]));
	printf("sizeof(p[5]) = %lu\n", _size_right(&p[5]));
	free(q);
	free(r);
	return 0;
}
