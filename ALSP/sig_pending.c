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
	sigset_t	sigset_mask, sigset_oldmask, sigset_pend;

	memset(&sa_usr1, 0, sizeof(struct sigaction));
	sa_usr1.sa_handler = sa_handler_usr;
	sigfillset(&sa_usr1.sa_mask);
	sigaction(SIGUSR1, &sa_usr1, NULL);
	memset(&sa_usr2, 0, sizeof(struct sigaction));
	sa_usr2.sa_handler = SIG_IGN;
	sigaction(SIGUSR2, &sa_usr2, NULL);
	sigfillset(&sigset_mask);
	sigdelset(&sigset_mask, SIGINT);
	printf("PID(%d)\n", getpid());

	for (;;) {
		printf("Install signal block mask (allow only SIGINT) \n");
		sigprocmask(SIG_SETMASK, &sigset_mask, &sigset_oldmask);
		sleep(10);
		sigpending(&sigset_pend);
		for (i = 1; i < SIGRTMIN; i++) {
			if (sigismember(&sigset_pend, i)) {
				printf("\tPending signal = %d\n", i);
				switch (i) {
					case SIGUSR2:
						sa_handler_usr(SIGUSR2);
						break;
					default:
						break;
				}
			}
		}
		printf("Restore the previous signal block mask.\n");
		sigprocmask(SIG_SETMASK, &sigset_oldmask, NULL);
	}

	return 0;
}

void sa_handler_usr(int signum)
{
	for (int i = 0; i < 3; i++) {
		printf("\tSignal(%s):%d sec.\n", signum == SIGUSR1 ? "USR1" : "USR2", i);
		sleep(1);
	}
}
