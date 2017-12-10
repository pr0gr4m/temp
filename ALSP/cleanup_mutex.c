#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "stdalsp.h"

#define NUM_THREADS	7

struct thread_arg
{
	pthread_t	tid;
	int			idx;
} *t_arg;

pthread_mutex_t	mutex;

void *start_thread(void *);
void clean_thread(struct thread_arg *);
void cleanup_mutex(void *);

int main()
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));
	pthread_mutex_init(&mutex, NULL);
	for (i = 0; i < NUM_THREADS; i++) {
		t_arg[i].idx = i;
		if ((ret = pthread_create(&t_arg[i].tid, NULL,
						start_thread, (void *)&t_arg[i]))) {
			pr_err("pthread_create : %s", strerror(ret));
			return 0;
		}
	}
	clean_thread(t_arg);
	return 0;
}

void *start_thread(void *arg)
{
	struct thread_arg	*t_arg = (struct thread_arg *)arg;
	int		ret;
	sleep(t_arg->idx);
	if ((ret = pthread_mutex_lock(&mutex))) {
		printf("\tThread:%d : Error : %s\n", t_arg->idx, strerror(ret));
	}
	pthread_cleanup_push(cleanup_mutex, (void *)&mutex);
	pr_out("[Thread:%d] holding mutex", t_arg->idx);
	sleep(1);
	sleep(1);
	sleep(1);
	sleep(1);
	pthread_cleanup_pop(0);
	pthread_mutex_unlock(&mutex);
	if (t_arg->idx == 1) {
		pr_out("[Thread:%d] pthread_cancel(Thread:2)", t_arg->idx);
		sleep(1);
		pthread_cancel( (t_arg + 1)->tid );
	}
	return t_arg;
}

void clean_thread(struct thread_arg *t_arg)
{
	int		i;
	struct thread_arg	*t_arg_ret;
	for (i = 0; i < NUM_THREADS; i++, t_arg++) {
		pthread_join(t_arg->tid, (void **)&t_arg_ret);
		pr_out("pthread_join : %d - %lu", t_arg->idx, t_arg->tid);
	}
}

void cleanup_mutex(void *arg)
{
	pthread_mutex_t	*lock = (pthread_mutex_t *)arg;
	pr_out("cleanup : mutex lock");
	pthread_mutex_unlock(lock);
}
