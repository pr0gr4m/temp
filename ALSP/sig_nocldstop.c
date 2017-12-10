#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void sa_handler_chld(int signum);

int main()
{
	int		ret;
	struct sigaction	sa_chld;
	memset(&sa_chld, 0, sizeof(struct sigaction));
	sa_chld.sa_handler = sa_handler_chld;
	sigfillset(&sa_chld.sa_mask);
	sa_chld.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sa_chld, NULL);
	printf("[MAIN] SIGNAL Handler installed\n");

	switch ((ret = fork())) {
		case 0:
			pause();
			exit(EXIT_SUCCESS);
		case -1:
			break;
		default:
			printf("- Child pid = %d \n", ret);
			break;
	}

	while (1) {
		pause();
		printf("[MAIN] Recv SIGNAL... \n");
	}

	return EXIT_SUCCESS;
}

void sa_handler_chld(int signum)
{
	pid_t	pid_child;
	int		status;

	printf("[SIGNAL] RECV SIGCHLD signal\n");
	while (1) {
		if ((pid_child = waitpid(-1, &status, WNOHANG)) > 0) {
			printf("\t- Exit child PID(%d) \n", pid_child);
		} else {
			break;
		}
	}
}
