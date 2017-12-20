#include <stdio.h>

int main(void)
{
	char buf[16];
	printf("Please enter something: ");
	gets(buf);
	printf("You entered '%s'\n", buf);
	return 0;
}
