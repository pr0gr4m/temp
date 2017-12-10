#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void sa_handler_usr(int signum);

int main()
{
	int		i;
	struct sigaction	sa_usr1, sa_usr2;
	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = sa_handler_usr;
	sigfillset(&sa_usr1.sa_mask);
	memset(&sa_usr2, 0, sizeof(struct sigaction));
	sa_usr2.sa_handler = sa_handler_usr;
	sigemptyset(&sa_usr2.sa_mask);

	sigaction(SIGUSR1, &sa_usr1, NULL);
	sigaction(SIGUSR2, &sa_usr2, NULL);
	printf("++ PID(%d) PGID(%d) SID(%d) \n", getpid(), getpgid(0), getsid(0));
	for (i = 0; i < 3; i++) {
		if (fork() == 0)
			break;
	}

	for (;;) {
		pause();
		printf("PID(%d) Recv SIGNAL... \n", getpid());
	}
	return EXIT_SUCCESS;
}

void sa_handler_usr(int signum)
{
	switch (signum) {
		case SIGUSR1:
			printf("-- PID(%d) PGID(%d) SID(%d) \n", getpid(), getpgid(0), getsid(0));
			break;
		case SIGUSR2:
			if (getpid() != getpgid(0)) {
				setpgid(0, 0);
			} else {
				setpgid(0, getppid());
			}
			printf("-- PID(%d) to PGID(%d) \n", getpid(), getpgid(0));
			break;
	}
}
