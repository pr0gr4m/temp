#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define NUM_THREADS	5

void *start_sigthread(void *);
void *start_thread(void *);

struct thread_arg
{
	pthread_t	pt_id;
	void *(*func)();
} thr_arg[NUM_THREADS] = {
	{ 0, start_sigthread },
	{ 0, start_thread },
	{ 0, start_thread },
	{ 0, NULL }
};

void clean_thread(struct thread_arg *t_arg);
void sa_handler_usr(int signum);

int main()
{
	int		i;
	sigset_t	sigset_mask;
	
	sigfillset(&sigset_mask);
	sigdelset(&sigset_mask, SIGINT);
	pthread_sigmask(SIG_SETMASK, &sigset_mask, NULL);
	printf("* Process PID = %d \n", getpid());
	for (i = 0; i < NUM_THREADS && thr_arg[i].func != NULL; i++) {
		if (pthread_create(&thr_arg[i].pt_id, NULL, thr_arg[i].func, (void *)&thr_arg[i])) {
			fprintf(stdout, "[MAIN] FAIL: pthread_create() \n");
			return 0;
		}
		printf(" Create thread : tid = %lu \n", thr_arg[i].pt_id);
	}
	clean_thread(thr_arg);
	return 0;
}

void *start_sigthread(void *arg)
{
	struct thread_arg	*t_arg = (struct thread_arg *)arg;
	sigset_t	sigset_mask;
	int		signum, ret_errno;

	printf("* Start signal thread (tid = %lu) \n", (long)pthread_self());
	sigemptyset(&sigset_mask);
	sigaddset(&sigset_mask, SIGUSR1);
	sigaddset(&sigset_mask, SIGUSR2);
	sigaddset(&sigset_mask, SIGTERM);

	while (1) {
		if ((ret_errno = sigwait(&sigset_mask, &signum))) {
			printf("FAIL:sigwait(%s)\n", strerror(ret_errno));
		}
		switch (signum) {
			case SIGUSR1: case SIGUSR2:
				sa_handler_usr(signum);
				break;
			case SIGTERM:
				printf("[SIGNAL] SIGTERM \n");
				exit(EXIT_SUCCESS);
			default:
				break;
		}
	}
	return t_arg;
}

void *start_thread(void *arg)
{
	struct thread_arg	*t_arg = (struct thread_arg *)arg;
	int		i;
	for (i = 0; ; i++)
		sleep(1);
	return t_arg;
}

void sa_handler_usr(int signum)
{
	int		i;
	for (i = 0; i < 5; i++) {
		printf("\t[%ld] Signal(%s):%d sec.\n", pthread_self(),
				signum == SIGUSR1 ? "USR1" : "USR2", i);
		sleep(1);
	}
}

void clean_thread(struct thread_arg *t_arg)
{
	int		i;
	struct thread_arg	*t_arg_ret;
	for (i = 0; i < NUM_THREADS && t_arg->func != NULL; i++, t_arg++) {
		pthread_join(t_arg->pt_id, (void **)&t_arg_ret);
		printf("+ Thread id(%lu)\n", t_arg->pt_id);
	}
}
