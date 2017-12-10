#include "apue.h"
#include <sys/wait.h>

int
main(int argc, char *argv[])
{
	pid_t pid = fork();

	if (pid == 0)
	{
		if ((pid = fork()) > 0)
		{
			printf("first child's id: %ld\n", (long)getpid());
			puts("end first child");
			exit(0);
		}

		sleep(2);
		printf("second child's id: %ld\n", (long)getpid());
		printf("second child's parent id: %ld\n", (long)getppid());
		puts("end second child");
		exit(0);
	}
	else
	{
		printf("Child Process ID: %d\n", pid);

		if (waitpid(pid, NULL, 0) != pid)
			exit(-1);

		sleep(10);
		puts("end parent");
		exit(0);
	}


	return 0;
}
