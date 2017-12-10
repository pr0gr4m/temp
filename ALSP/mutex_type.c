#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "stdalsp.h"

#define NUM_THREADS	4

struct thread_arg {
	pthread_t	tid;
	int			idx;
} *t_arg;

pthread_mutex_t	mutex;
pthread_mutexattr_t	mutexattr;

void *start_thread(void *);
void clean_thread(struct thread_arg *);

int main()
{
	int		i, ret;
	t_arg = (struct thread_arg *)calloc(NUM_THREADS, sizeof(struct thread_arg));
	pthread_mutexattr_init(&mutexattr);
#if defined(NORMAL_MUTEX)
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_NORMAL);
#elif defined(RECURSIVE_MUTEX)
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
#elif defined(ERRORCHECK_MUTEX)
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
#endif
	pthread_mutex_init(&mutex, &mutexattr);

	for (i = 0; i < NUM_THREADS; i++) {
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
	int		ret;
	if ((ret = pthread_mutex_lock(&mutex))) {
		if (ret == EDEADLK) {
			pr_err("\t lock : EDEADLK detected");
		} else {
			pr_err("\t lock (errno = %s)", strerror(ret));
		}
	}

	pr_out("[thread] idx(%d) tid(%ld)", t_arg->idx, pthread_self());
	sleep(t_arg->idx * 2);
	if (t_arg->idx > 1) {
		if ((ret = pthread_mutex_lock(&mutex))) {
			if (ret == EDEADLK) {
				pr_err("\t lock : EDEADLK detected");
			} else {
				pr_err("\t lock (errno = %s)", strerror(ret));
			}
		}
	}

	if ((ret = pthread_mutex_unlock(&mutex))) {
		pr_err("\t unlock: (errno = %s)", strerror(ret));
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
