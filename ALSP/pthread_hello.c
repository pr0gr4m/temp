#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "stdalsp.h"

#define NUM_THREADS	5

struct thread_arg {
	pthread_t	tid;
	int			idx;
} *t_arg;

void *start_thread(void *);
void clean_thread(struct thread_arg *);

int main()
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));

	for (i = 0; i < NUM_THREADS; ++i) {
		t_arg[i].idx = i;
		if ((ret = pthread_create(&t_arg[i].tid, NULL, start_thread, (void *)&t_arg[i]))) {
			pr_err("pthread_create : %s", strerror(ret));
			return 0;
		}
		pr_out("pthread_create : tid = %lu", t_arg[i].tid);
	}

	clean_thread(t_arg);
	return 0;
}

void *start_thread(void *arg)
{
	struct thread_arg *t_arg = (struct thread_arg *)arg;
	sleep(2);
	printf("\tHello I'm pthread(%d) - TID(%lu)\n", t_arg->idx, t_arg->tid);
	t_arg->idx += 10;
	pthread_exit(t_arg);
}

void clean_thread(struct thread_arg *t_arg)
{
	int		i;
	struct thread_arg *t_arg_ret;
	for (i = 0; i < NUM_THREADS; i++, t_arg++) {
		pthread_join(t_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread_join : %d - %lu", t_arg->idx, t_arg->tid);
	}
}
