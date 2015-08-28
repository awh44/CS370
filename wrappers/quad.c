#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Please include the pid as a command line parameter.\n");
		return 1;
	}

	pid_t pid = atol(argv[1]);
	long timeslice = syscall(287, pid);
	if (timeslice < 0)
	{        
		printf("Failure! (%d)\n", timeslice);  
		return 2;
	}

	printf("Success! New timeslice: %ld\n", timeslice);
	return 0;
}
