#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main()
{
	printf("Child[%d]: Start\n", getpid());
	printf("Child[%d]: raise(SIGUSR1)\n", getpid());
	raise(SIGUSR1);
	printf("Child[%d]: raise(SIGUSR2)\n", getpid());
	raise(SIGUSR2);
	printf("Child[%d]: raise(SIGTERM) \n", getpid());
	raise(SIGTERM);
	printf("Child[%d]: Exit\n", getpid());
	return 0;
}
