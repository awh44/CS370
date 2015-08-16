#include <stdio.h>

int main(void)
{
	int syscallnum = 285;
	printf("Process ID: %d\n", syscall(syscallnum));
	return 0;
}
