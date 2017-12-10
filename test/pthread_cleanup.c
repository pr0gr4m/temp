#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void cleanup(void *arg)
{
	printf("cleanup : %s\n", (char *)arg);
}

void *thr_fn1(void *arg)
{
	printf("thread 1 start \n");
	pthread_cleanup_push(cleanup, "Thread 1 first handler");
	pthread_cleanup_push(cleanup, "Thread 1 second handler");
	printf("thread 1 push complete \n");
	if (arg)
		return ((void*)1);

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);

	return ((void*)2);
}

void *thr_fn2(void *arg)
{
	printf("thread 2 start \n");

	pthread_cleanup_push(cleanup, "Thread 2 first handler");
	pthread_cleanup_push(cleanup, "Thread 2 second handler");

	printf("thread 2 push comlete \n");

	if (arg)
		pthread_exit((void *)3);

	pthread_cleanup_pop(1);
	pthread_cleanup_pop(1);
	return ((void *)4);
	
}

int main()
{
	int err;
	pthread_t tid1, tid2;
	void *ret;

	err = pthread_create(&tid1, NULL, thr_fn1, (void *)1);
	if (err != 0)
		exit(EXIT_FAILURE);

	err = pthread_create(&tid2, NULL, thr_fn2, (void *)1);
	if (err != 0)
		exit(EXIT_FAILURE);

	err = pthread_join(tid1, &ret);
	if (err != 0)
		exit(EXIT_FAILURE);

	printf("thread 1 exit code : %d \n", (int)ret);

	err = pthread_join(tid2, &ret);
	if (err != 0)
		exit(EXIT_FAILURE);

	printf("thread 2 exit code : %d \n", (int)ret);

	return 0;
}
