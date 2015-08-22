#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		return 1;
	}

	pid_t target = atol(argv[1]), victim = atol(argv[2]);
	
	long success = syscall(288, target, victim);
	if (success < 0)
	{
		printf("Failure!\n");
		return 2;
	}

	printf("Success! Stolen time: %ld\n", success);
	return 0;
}
