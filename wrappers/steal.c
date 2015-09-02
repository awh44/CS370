#include <errno.h>
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
	long success = syscall(286, pid);
	if (success < 0)
	{        
		perror("Failure in steal");  
		return 2;
	}


	printf("Success!\n");
	return 0;
} 
