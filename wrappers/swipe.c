#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Please include the target and the victim as command line parameters.\n");
		return 1;
	}

	pid_t target = atol(argv[1]), victim = atol(argv[2]);
	
	long success = syscall(288, target, victim);
	if (success < 0)
	{
		perror("Failure in swipe");
		return 2;
	}

	printf("Success! Stolen time: %ld\n", success);
	return 0;
}
