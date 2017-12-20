#include <stdio.h>
#include <string.h>

int main(void)
{
	char buf[16];
	for(int i = 0; i < 16; i++)
		buf[i] = 'A' + i;
	printf("length: %lu\n", strlen(buf));
	buf[4] = 0;
	printf("length: %lu\n", strlen(buf));
	return 0;
}
