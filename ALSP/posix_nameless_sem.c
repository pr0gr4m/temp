#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define	MAX_CHILD_THREAD	30

sem_t	psem;

typedef struct thread_arg {
	pthread_t	tid;
	int			idx;
} thread_arg;

thread_arg thr_arg[MAX_CHILD_THREAD];
void *start_child(void *);

int main(int argc, char *argv[])
{
	int i, n_count, sem_value;

	if (argc != 2) {
		printf("%s [counter] \n", argv[0]);
		exit(EXIT_FAILURE);
	}

	n_count = atoi(argv[1]);
	if (sem_init(&psem, 0, n_count) != 0) {
		perror("sem_init error");
		exit(EXIT_FAILURE);
	}

	sem_getvalue(&psem, &sem_value);
	printf("[%d] sem_gevalue = %d\n", getpid(), sem_value);
	
	for (i = 0; i < MAX_CHILD_THREAD; i++) {
		printf("[%d] iteration(%d) : Atomically decrease\n", getpid(), i);
		sem_wait(&psem);
		thr_arg[i].idx = i;
		if (pthread_create(&thr_arg[i].tid, NULL, start_child, &thr_arg[i]) != 0) {
			perror("pthread_create error");
			exit(EXIT_FAILURE);
		}

		usleep(10000);
	}

	for (i = 0; i < MAX_CHILD_THREAD; i++) {
		pthread_join(thr_arg[i].tid, NULL);
	}

	sem_getvalue(&psem, &sem_value);
	printf("[%d] sem_getvalue = %d\n", getpid(), sem_value);
	if (sem_destroy(&psem) == -1) {
		perror("FAIL: sem_destroy");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void *start_child(void *arg)
{
	thread_arg	*t_arg = (thread_arg *)arg;
	if (t_arg->idx == 11)
		return NULL;
	printf("\t[Child thread:%d] sleep(2)\n", t_arg->idx);
	sleep(2);
	sem_post(&psem);
	return 0;
}
