#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Please include the filename to which to write.\n");
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		printf("Error opening file.\n");
		return 2;
	}

	char buff[] = "Hello, how are you?";
	long success = syscall(291, fd, buff, sizeof(buff) - 1);
	if (success < 0)
	{
		perror("Failure in forcewrite");
		return 3;
	}

	printf("Success!\n");
	return 0;
}
